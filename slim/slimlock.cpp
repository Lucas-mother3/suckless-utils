/* slimlock
 * Copyright (c) 2010-2012 Joel Burget <joelburget@gmail.com>
 * Copyright (c) 2022-2023 Rob Pearce <slim@flitspace.org.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <cstdio>
#include <cstring>
#include <algorithm>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/vt.h>
#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/dpms.h>
#include <security/pam_appl.h>
#include <pthread.h>
#include <err.h>
#include <signal.h>
#include <unistd.h>			// for usleep
#include <sys/stat.h>
#include <sys/file.h>
#include <errno.h>
#include <sys/file.h>
#include <fcntl.h>

#include "cfg.h"
#include "util.h"
#include "panel.h"
#include "const.h"

#undef APPNAME
#define APPNAME "slimlock"
#define SLIMLOCKCFG SYSCONFDIR"/slimlock.conf"

using namespace std;

bool AuthenticateUser();
static int ConvCallback(int num_msgs, const struct pam_message **msg,
						struct pam_response **resp, void *appdata_ptr);
void HandleSignal(int sig);
void *RaiseWindow(void *data);

// In the absence of a class instance to contain these, just make them file
// public, as they're needed by multiple functions
static Display* Dpy;
static int Scr;
static Window Root;
static Window win;
static Cfg* cfg;
static Panel* LoginPanel;

static pam_handle_t *pam_handle;

static const struct pam_conv conv = {ConvCallback, NULL};

static CARD16 dpms_standby, dpms_suspend, dpms_off, dpms_level;
static BOOL dpms_state, using_dpms;

static int term;	// Used by a C callback function


static void die(const char *errstr, ...)
{
	va_list ap;

	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	if((argc == 2) && !strcmp("-v", argv[1]))
		die ( APPNAME "-" VERSION ", © 2010-2012 Joel Burget\nUpdates © 2022-2023 Rob Pearce\n" );
	else if(argc != 1)
		die ( "usage: " APPNAME " [-v]\n" );

	void (*prev_fn)(int);

	// restore DPMS settings should slimlock be killed in the line of duty
	prev_fn = signal(SIGTERM, HandleSignal);
	if (prev_fn == SIG_IGN)
		signal(SIGTERM, SIG_IGN);

	// create a lock file to solve mutliple instances problem
	// /var/lock used to be the place to put this, now it's /run/lock
	// ...i think
	struct stat statbuf;
	int lock_file;

	// try /run/lock first, since i believe it's preferred
	if (!stat("/run/lock", &statbuf))
		lock_file = open ( "/run/lock/" APPNAME ".lock", O_CREAT | O_RDWR, 0666);
	else
		lock_file = open ( "/var/lock/" APPNAME ".lock", O_CREAT | O_RDWR, 0666);

	int rc = flock(lock_file, LOCK_EX | LOCK_NB);

	if(rc) {
		if(EWOULDBLOCK == errno)
			die(APPNAME" already running\n");
	}

	unsigned int cfg_passwd_timeout;

	/* Read configuration and theme */
	cfg = new Cfg;
	cfg->readConf(CFGFILE);
	cfg->readConf(SLIMLOCKCFG);
	string themeName = "";
	string themebase = "";
	string themefile = "";
	string themedir = "";

	themebase = string(THEMESDIR) + "/";
	themeName = cfg->getOption("current_theme");
	string::size_type pos;
	if ((pos = themeName.find(",")) != string::npos)
	{
		/* input is a set */
		themeName = cfg->findValidRandomTheme(themeName);
	}

	bool loaded = false;
	while (!loaded)
	{
		themedir =  themebase + themeName;
		themefile = themedir + THEMESFILE;
		if (!cfg->readConf(themefile))
		{
			if (themeName == "default")
			{
				cerr << APPNAME << ": Failed to open default theme file "
					 << themefile << endl;
				exit(ERR_EXIT);
			}
			else
			{
				cerr << APPNAME << ": Invalid theme in config: "
					 << themeName << endl;
				themeName = "default";
			}
		}
		else
		{
			loaded = true;
		}
	}

	const char *DisplayName = getenv("DISPLAY");
	if (!DisplayName)
		DisplayName = DISPLAY;

	Dpy = XOpenDisplay(DisplayName);
	if ( Dpy == 0 )
		die(APPNAME": cannot open display\n");

	/* Get screen and root window */
	Scr = DefaultScreen(Dpy);
	Root = RootWindow(Dpy, Scr);

	XSetWindowAttributes wa;
	wa.override_redirect = 1;
	wa.background_pixel = BlackPixel(Dpy, Scr);

	// Create a full screen window
	win = XCreateWindow(Dpy, Root,
			0, 0, DisplayWidth(Dpy, Scr), DisplayHeight(Dpy, Scr),
			0, DefaultDepth(Dpy, Scr), CopyFromParent,
			DefaultVisual(Dpy, Scr), CWOverrideRedirect | CWBackPixel,
			&wa);
	XMapWindow(Dpy, win);

	XFlush(Dpy);
	for (int len = 1000; len; len--) {
		if(XGrabKeyboard(Dpy, Root, True, GrabModeAsync, GrabModeAsync, CurrentTime)
			== GrabSuccess)
			break;
		usleep(1000);
	}
	XSelectInput(Dpy, win, ExposureMask | KeyPressMask);

	/* Create panel */
	LoginPanel = new Panel(Dpy, Scr, win, cfg, themedir, Panel::Mode_Lock);
	LoginPanel->HideCursor();

	int ret = pam_start(APPNAME, LoginPanel->GetName().c_str(), &conv, &pam_handle);
	// If we can't start PAM, just exit because slimlock won't work right
	if (ret != PAM_SUCCESS)
		die("PAM: %s\n", pam_strerror(pam_handle, ret));

	// disable tty switching
	if(cfg->getOption("tty_lock") == "1") {
		if ((term = open("/dev/console", O_RDWR)) == -1)
			perror("error opening console");

		if ((ioctl(term, VT_LOCKSWITCH)) == -1)
			perror("error locking console");
	}

	// Set up DPMS
	unsigned int cfg_dpms_standby, cfg_dpms_off;
	cfg_dpms_standby = Cfg::string2int(cfg->getOption("dpms_standby_timeout").c_str());
	cfg_dpms_off = Cfg::string2int(cfg->getOption("dpms_off_timeout").c_str());
	using_dpms = DPMSCapable(Dpy) && (cfg_dpms_standby > 0);
	if (using_dpms) {
		DPMSGetTimeouts(Dpy, &dpms_standby, &dpms_suspend, &dpms_off);

		DPMSSetTimeouts(Dpy, cfg_dpms_standby,
						cfg_dpms_standby, cfg_dpms_off);

		DPMSInfo(Dpy, &dpms_level, &dpms_state);
		if (!dpms_state)
			DPMSEnable(Dpy);
	}

	// Get password timeout
	cfg_passwd_timeout = Cfg::string2int(cfg->getOption("wrong_passwd_timeout").c_str());
	// Let's just make sure it has a sane value
	cfg_passwd_timeout = cfg_passwd_timeout > 60 ? 60 : cfg_passwd_timeout;

	pthread_t raise_thread;
	pthread_create(&raise_thread, NULL, RaiseWindow, NULL);

#if 0	// The DM code does this:
			/* Init Root */
			LoginPanel->setBackground();

			/* Show panel */
			LoginPanel->OpenPanel();
#endif

	// Main loop
	while (true)
	{
		LoginPanel->ResetPasswd();

		// AuthenticateUser returns true if authenticated
		if (AuthenticateUser())
			break;

		LoginPanel->WrongPassword(cfg_passwd_timeout);
	}

	// kill thread before destroying the window that it's supposed to be raising
	pthread_cancel(raise_thread);

	LoginPanel->ClosePanel();
	delete LoginPanel;

	// Get DPMS stuff back to normal
	if (using_dpms) {
		DPMSSetTimeouts(Dpy, dpms_standby, dpms_suspend, dpms_off);
		// turn off DPMS if it was off when we entered
		if (!dpms_state)
			DPMSDisable(Dpy);
	}

	XCloseDisplay(Dpy);

	close(lock_file);	// will inherently release the flock

	if(cfg->getOption("tty_lock") == "1") {
		if ((ioctl(term, VT_UNLOCKSWITCH)) == -1) {
			perror("error unlocking console");
		}
	}
	close(term);

	return 0;
}


static int ConvCallback(int num_msgs, const struct pam_message **msg,
						struct pam_response **resp, void *appdata_ptr)
{
	LoginPanel->EventHandler(Panel::Get_Passwd);

	// PAM expects an array of responses, one for each message
	if (num_msgs == 0 ||
		(*resp = (pam_response*) calloc(num_msgs, sizeof(struct pam_message))) == NULL)
		return PAM_BUF_ERR;

	for (int i = 0; i < num_msgs; i++) {
		if (msg[i]->msg_style != PAM_PROMPT_ECHO_OFF &&
			msg[i]->msg_style != PAM_PROMPT_ECHO_ON)
			continue;

		// return code is currently not used but should be set to zero
		resp[i]->resp_retcode = 0;
		if ((resp[i]->resp = strdup(LoginPanel->GetPasswd().c_str())) == NULL) {
			free(*resp);
			return PAM_BUF_ERR;
		}
	}

	return PAM_SUCCESS;
}

bool AuthenticateUser()
{
	return(pam_authenticate(pam_handle, 0) == PAM_SUCCESS);
}


void HandleSignal(int sig)
{
	// Get DPMS stuff back to normal
	if (using_dpms) {
		DPMSSetTimeouts(Dpy, dpms_standby, dpms_suspend, dpms_off);
		// turn off DPMS if it was off when we entered
		if (!dpms_state)
			DPMSDisable(Dpy);
	}

	if ((ioctl(term, VT_UNLOCKSWITCH)) == -1) {
		perror("error unlocking console");
	}
	close(term);

	LoginPanel->ClosePanel();
	delete LoginPanel;

	die(APPNAME": Caught signal; dying\n");
}

void* RaiseWindow(void *data)
{
	while(1) {
		XRaiseWindow(Dpy, win);
		sleep(1);
	}

	return (void *)0;
}
