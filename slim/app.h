/* SLiM - Simple Login Manager
 *  Copyright (C) 1997, 1998 Per Liden
 *  Copyright (C) 2004-06 Simone Rota <sip@varlock.com>
 *  Copyright (C) 2004-06 Johannes Winkelmann <jw@tks6.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#ifndef _APP_H_
#define _APP_H_

#include <X11/Xlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <setjmp.h>
#include <stdlib.h>
#include <iostream>
#include "panel.h"
#include "cfg.h"
#include "image.h"

#ifdef USE_PAM
#include "PAM.h"
#endif
#ifdef USE_CONSOLEKIT
#include "Ck.h"
#endif

class App
{
public:
	App(int argc, char **argv);
	~App();
	void Run();
	int GetServerPID();
	void RestartServer();
	void StopServer();

	/* Lock functions */
	void GetLock();
	void RemoveLock();

	bool isServerStarted();

private:
	void Login();
	void Reboot();
	void Halt();
	void Suspend();
	void Console();
	void Exit();
	void KillAllClients(Bool top);
	void ReadConfig();
	void OpenLog();
	void CloseLog();
	void CreateServerAuth();
	char *StrConcat(const char *str1, const char *str2);
	void UpdatePid();

	bool AuthenticateUser(bool focuspass);

	static void replaceVariables(std::string &input,
								 const std::string &var,
								 const std::string &value);

	/* Server functions */
	int StartServer();
	int ServerTimeout(int timeout, char *string);
	int WaitForServer();

	/* Private data */
	Window Root;	///< The root window of the default screen, on which to draw
	Display *Dpy;	///< Connection to the X-server
	int Scr;		///< Which "screen" to use (which will be the default one)
	Panel *LoginPanel;	///< The panel we display and interact through
	int ServerPID;	///< Process ID of the X-server we launched
	const char *DisplayName;	///< The display to request, usually ":0.0"
	bool serverStarted;	///< Whether we (think we) have started an X server

#ifdef USE_PAM
	PAM::Authenticator pam;	///< Interface to the PAM authentication library
#endif
#ifdef USE_CONSOLEKIT
	Ck::Session ck;		///< Interface to ConsoleKit, if used
#endif

	/* Options */
	Cfg *cfg;		///< Collection of options from the configuration file

	void blankScreen();

	bool firstlogin;	///< Whether to exhibit first login behaviour, or repeat
	bool daemonmode;	///< Are we running as a daemon?
	bool force_nodaemon;	///< Are we forced NOT to be a daemon?
	/* For testing themes */
	char *testtheme;	///< Name of the theme to test, from command line
	bool testing;		///< Whether we're running in theme testing mode
	short tww, twh;		///< The user's requested test window size

#ifdef USE_CONSOLEKIT
	bool consolekit_support_enabled;	///< Whether to use ConsoleKit (not compatible with systemd)
#endif

	std::string themeName;	///< Name of the theme in use
	std::string mcookie;	///< Randomly generated X auth cookie

	const int mcookiesize;
};

#endif /* _APP_H_ */
