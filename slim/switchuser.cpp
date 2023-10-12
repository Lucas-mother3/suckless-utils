/*
 * SLiM - Simple Login Manager
 *  Copyright (C) 1997, 1998 Per Liden
 *  Copyright (C) 2004-06 Simone Rota <sip@varlock.com>
 *  Copyright (C) 2004-06 Johannes Winkelmann <jw@tks6.net>
 *  Copyright (C) 2022 Rob Pearce <slim@flitspace.org.uk>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#include <unistd.h>
#include <cstdio>
#include "const.h"
#include "cfg.h"
#include "log.h"
#include "util.h"
#include "switchuser.h"

using namespace std;

SwitchUser::SwitchUser(struct passwd *pw, Cfg *c, const string& display,
					   char** _env)
	: cfg(c), Pw(pw), env(_env)
{
}

SwitchUser::~SwitchUser()
{
	/* Never called */
}

void SwitchUser::Login(const char* cmd, const char* mcookie)
{
	SetUserId();
	SetClientAuth(mcookie);
	Execute(cmd);
}

void SwitchUser::SetUserId()
{
	if( (Pw == 0) ||
			(initgroups(Pw->pw_name, Pw->pw_gid) != 0) ||
			(setgid(Pw->pw_gid) != 0) ||
			(setuid(Pw->pw_uid) != 0) ) {
		logStream << APPNAME << ": could not switch user id" << endl;
		exit(ERR_EXIT);
	}
}

void SwitchUser::Execute(const char* cmd)
{
	if ( chdir(Pw->pw_dir) < 0 )
		logStream << APPNAME << ": unable to chdir() to user's home: " << strerror(errno) << endl;
	logStream.closeLog();
	execle(Pw->pw_shell, Pw->pw_shell, "-c", cmd, NULL, env);
	/// @todo this copy-paste of App:CloseLog() should be cleaned up
	if ( !logStream.openLog( cfg->getOption("logfile").c_str() ) ) {
		cerr <<  APPNAME << ": Could not access log file: " << cfg->getOption("logfile") << endl;
		exit(ERR_EXIT);
	}
	logStream << APPNAME << ": could not execute login command: " << strerror(errno) << endl;
}

void SwitchUser::SetClientAuth(const char* mcookie)
{
	string home = string(Pw->pw_dir);
	string authfile = home + "/.Xauthority";
	remove(authfile.c_str());
	Util::add_mcookie(mcookie, ":0", cfg->getOption("xauth_path"),
	  authfile);
}
