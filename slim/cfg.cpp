/* SLiM - Simple Login Manager
 *  Copyright (C) 2004-06 Simone Rota <sip@varlock.com>
 *  Copyright (C) 2004-06 Johannes Winkelmann <jw@tks6.net>
 *  Copyright (C) 2012-13 Nobuhiro Iwamatsu <iwamatsu@nigauri.org>
 *  Copyright (C) 2022-23 Rob Pearce <slim@flitspace.org.uk>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#include <fstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "util.h"	// for Util::random
#include "log.h"	// for logStream

#include "cfg.h"

using namespace std;

typedef pair<string,string> option;

/**
 * Constructor: creates the Cfg object and populates the available options
 * with default values.
 */
Cfg::Cfg()
	: currentSession(-1)
{
	/* Configuration options */
	options.insert(option("default_path","/bin:/usr/bin:/usr/local/bin"));
	options.insert(option("default_xserver","/usr/bin/X"));
	options.insert(option("xserver_arguments",""));
	options.insert(option("numlock",""));
	options.insert(option("daemon",""));
	options.insert(option("xauth_path","/usr/bin/xauth"));
	options.insert(option("login_cmd","exec /bin/bash -login ~/.xinitrc %session"));
	options.insert(option("halt_cmd","/sbin/shutdown -h now"));
	options.insert(option("reboot_cmd","/sbin/shutdown -r now"));
	options.insert(option("suspend_cmd",""));
	options.insert(option("sessionstart_cmd",""));
	options.insert(option("sessionstop_cmd",""));
	options.insert(option("console_cmd","/usr/bin/xterm -C -fg white -bg black +sb -g %dx%d+%d+%d -fn %dx%d -T ""Console login"" -e /bin/sh -c ""/bin/cat /etc/issue; exec /bin/login"""));
	options.insert(option("screenshot_cmd","import -window root /slim.png"));
	options.insert(option("default_user",""));
	options.insert(option("focus_password","no"));
	options.insert(option("auto_login","no"));
	options.insert(option("current_theme","default"));
	options.insert(option("lockfile","/var/run/slim.lock"));
	options.insert(option("logfile","/var/log/slim.log"));
	options.insert(option("authfile","/var/run/slim.auth"));
	options.insert(option("shutdown_msg","The system is halting..."));
	options.insert(option("reboot_msg","The system is rebooting..."));
	options.insert(option("sessions", ""));
	options.insert(option("sessiondir",""));
	options.insert(option("hidecursor","false"));

	/* Theme stuff */
	options.insert(option("background_style","stretch"));
	options.insert(option("background_color","#CCCCCC"));

	options.insert(option("input_panel_x","50%"));	/* Panel position on screen */
	options.insert(option("input_panel_y","40%"));
	options.insert(option("input_font","Verdana:size=11"));
	options.insert(option("input_color", "#000000"));
	options.insert(option("input_shadow_xoffset", "0"));
	options.insert(option("input_shadow_yoffset", "0"));
	options.insert(option("input_shadow_color","#FFFFFF"));
	options.insert(option("input_name_x","200"));	/* relative to panel */
	options.insert(option("input_name_y","154"));
	options.insert(option("input_pass_x","-1")); /* default is single inputbox */
	options.insert(option("input_pass_y","-1"));

	options.insert(option("welcome_msg","Welcome to %host"));
	options.insert(option("welcome_font","Verdana:size=14"));
	options.insert(option("welcome_color","#FFFFFF"));
	options.insert(option("welcome_x","-1"));
	options.insert(option("welcome_y","-1"));
	options.insert(option("welcome_shadow_xoffset", "0"));
	options.insert(option("welcome_shadow_yoffset", "0"));
	options.insert(option("welcome_shadow_color","#FFFFFF"));

	options.insert(option("username_msg","Please enter your username"));
	options.insert(option("username_font","Verdana:size=12"));
	options.insert(option("username_color","#FFFFFF"));
	options.insert(option("username_x","-1"));
	options.insert(option("username_y","-1"));
	options.insert(option("username_shadow_xoffset", "0"));
	options.insert(option("username_shadow_yoffset", "0"));
	options.insert(option("username_shadow_color","#FFFFFF"));

	options.insert(option("password_msg","Please enter your password"));
	options.insert(option("password_x","-1"));
	options.insert(option("password_y","-1"));

	options.insert(option("msg_font","Verdana:size=16:bold"));
	options.insert(option("msg_color","#FFFFFF"));
	options.insert(option("msg_x","40"));
	options.insert(option("msg_y","40"));
	options.insert(option("msg_shadow_xoffset", "0"));
	options.insert(option("msg_shadow_yoffset", "0"));
	options.insert(option("msg_shadow_color","#FFFFFF"));

	options.insert(option("session_msg","Session:"));
	options.insert(option("session_font","Verdana:size=16:bold"));
	options.insert(option("session_color","#FFFFFF"));
	options.insert(option("session_x","50%"));
	options.insert(option("session_y","90%"));
	options.insert(option("session_shadow_xoffset", "0"));
	options.insert(option("session_shadow_yoffset", "0"));
	options.insert(option("session_shadow_color","#FFFFFF"));

	// What to do if the authorisation fails
	options.insert(option("keep_user_on_fail", "0"));
	options.insert(option("wrong_passwd_timeout", "2"));
	options.insert(option("passwd_feedback_msg", "Authentication failed"));
	options.insert(option("passwd_feedback_capslock", "Authentication failed (CapsLock is on)"));
	options.insert(option("passwd_feedback_x", "-1"));	/* no feedback by default */
	options.insert(option("passwd_feedback_y", "-1"));
	options.insert(option("bell", "0"));

	// slimlock-specific options
	options.insert(option("dpms_standby_timeout", "60"));
	options.insert(option("dpms_off_timeout", "600"));
	options.insert(option("show_username", "1"));
	options.insert(option("show_welcome_msg", "0"));
	options.insert(option("tty_lock", "1"));

	error = "";
}

Cfg::~Cfg()
{
	options.clear();
}

/**
 * Parses known options from the given configfile / themefile
 * 
 * @param	configfile	Path to configuration or theme
 * @return				true on sucess, false if file not found
 */
bool Cfg::readConf(string configfile)
{
	int n = -1;
	size_t pos = 0;
	string line, next, op, fn(configfile);
	map<string,string>::iterator it;
	ifstream cfgfile(fn.c_str());

	if (!cfgfile)
	{
		error = "Cannot read configuration file: " + configfile;
		return false;
	}
	while (getline(cfgfile, line))
	{
		// New parser to fix ticket #4
		pos = line.length();
		if ( ( pos > 0 ) && ( line[pos-1] == '\\' ) )
		{
			line.replace ( pos-1, 1, " " );
			next = next + line;
			continue;
		}

		if ( !next.empty() )
		{
			line = next + line;
			next = "";
		}

		// Ignore blank lines and comment lines
		if ( line.empty() || line[0] == '#' )
			continue;

		// Now parse and assign
		if ( !parseOption ( line ) )
			cerr << error << '\n';	// not a fatal error
	}
	cfgfile.close();

	fillSessionList();

	return true;
}

/**
 *  Sets an option value from a line. Returns true on success.
 */
bool Cfg::parseOption ( string line )
{
	size_t pos = 0;
	const string delims = " \t";
	string name, value;

	// First split the line into a name/value pair
	pos = line.find_first_of ( delims );
	if ( pos == string::npos )
	{
		error = "Badly formed line: " + line;
		return false;
	}
	name = line.substr ( 0, pos );
	value = Trim ( line.substr ( pos ) );
	if ( value.empty() )
	{
		error = "Badly formed line: " + line;
		return false;
	}

	// Look to see if it's a known option
	if ( options.find ( name ) == options.end() )
	{
		error = "Unknown option name: " + name;
		return false;
	}
	// finally assign it
	options[name] = value;
	return true;
}


const string& Cfg::getError() const
{
	return error;
}

string& Cfg::getOption(string option)
{
	return options[option];
}

/* return a trimmed string */
string Cfg::Trim( const string& s )
{
	if ( s.empty() ) {
		return s;
	}
	int pos = 0;
	string line = s;
	int len = line.length();
	while ( pos < len && isspace( line[pos] ) ) {
		++pos;
	}
	line.erase( 0, pos );
	pos = line.length()-1;
	while ( pos > -1 && isspace( line[pos] ) ) {
		--pos;
	}
	if ( pos != -1 ) {
		line.erase( pos+1 );
	}
	return line;
}

/* Return the welcome message with replaced vars */
string Cfg::getWelcomeMessage()
{
	string s = getOption("welcome_msg");
	int n = s.find("%host");
	if (n >= 0) {
		string tmp = s.substr(0, n);
		char host[40];
		gethostname(host,40);
		tmp = tmp + host;
		tmp = tmp + s.substr(n+5, s.size() - n);
		s = tmp;
	}
	n = s.find("%domain");
	if (n >= 0) {
		string tmp = s.substr(0, n);
		char domain[40];
		if ( getdomainname(domain,40) == 0 )
			tmp = tmp + domain;
		else
			tmp = tmp + "<unknown domain>";
		tmp = tmp + s.substr(n+7, s.size() - n);
		s = tmp;
	}
	return s;
}

int Cfg::string2int(const char* string, bool* ok)
{
	char* err = 0;
	int l = (int)strtol(string, &err, 10);
	if (ok) {
		*ok = (*err == 0);
	}
	return (*err == 0) ? l : 0;
}

int Cfg::getIntOption(std::string option)
{
	return string2int(options[option].c_str());
}


/**
 * Get absolute position
 * 
 * Converts a config position string into absolute coordinates. If the string 
 * is a plain number, this is just an atoi but if there is a percentage sign 
 * then the value is converted using the size of the canvas and the object.
 * 
 * @param	position	Configured position as a string
 * @param	max			Size of canvas in the relevant axis
 * @param	width		Size of the object being placed
 * @return				Absolute coordinate to achieve placement
 */
int Cfg::absolutepos(const string& position, int max, int width)
{
	int n = position.find("%");
	if (n>0) { /* X Position expressed in percentage */
		int result = (max*string2int(position.substr(0, n).c_str())/100) - (width / 2);
		return result < 0 ? 0 : result ;
	} else { /* Absolute X position */
		return string2int(position.c_str());
	}
}

/* split a comma separated string into a vector of strings */
void Cfg::split(vector<string>& v, const string& str, char c, bool useEmpty)
{
	v.clear();
	string::const_iterator s = str.begin();
	string tmp;
	while (true) {
		string::const_iterator begin = s;
		while (*s != c && s != str.end())
		{
			++s;
		}
		tmp = string(begin, s);
		if (useEmpty || tmp.size() > 0)
			v.push_back(tmp);
		if (s == str.end()) {
			break;
		}
		if (++s == str.end()) {
			if (useEmpty)
				v.push_back("");
			break;
		}
	}
}

void Cfg::fillSessionList()
{
	string strSessionList = getOption("sessions");
	string strSessionDir  = getOption("sessiondir");

	sessions.clear();

	if ( !strSessionDir.empty() )
	{
		DIR *pDir = opendir(strSessionDir.c_str());

		if (pDir != NULL) {
			struct dirent *pDirent = NULL;

			while ((pDirent = readdir(pDir)) != NULL) {
				string strFile(strSessionDir);
				strFile += "/";
				strFile += pDirent->d_name;

				struct stat oFileStat;

				if (stat(strFile.c_str(), &oFileStat) == 0) {
					if (S_ISREG(oFileStat.st_mode) &&
							access(strFile.c_str(), R_OK) == 0){
						ifstream desktop_file(strFile.c_str());
						if (desktop_file){
							string line, session_name = "", session_exec = "";
							while (getline( desktop_file, line )) {
								if (line.substr(0, 5) == "Name=") {
									session_name = line.substr(5);
									if (!session_exec.empty())
										 break;
								}
								else if (line.substr(0, 5) == "Exec=") {
									session_exec = line.substr(5);
									if (!session_name.empty())
										break;
								}
							}
							desktop_file.close();
							if (!session_name.empty() && !session_exec.empty()) {
								pair<string,string> session(session_name,session_exec);
								sessions.push_back(session);
							} else if (access(strFile.c_str(), X_OK) == 0) {
								pair<string,string> session(string(pDirent->d_name),strFile);
								sessions.push_back(session);
							}
						}
					}
				}
			}
			closedir(pDir);
		}
	}

	if (sessions.empty())
	{
		if (strSessionList.empty())
		{
			pair<string,string> session("","");
			sessions.push_back(session);
		}
		else
		{
			// iterate through the split of the session list
			vector<string> sessit;
			split(sessit,strSessionList,',',false);
			for (vector<string>::iterator it = sessit.begin(); it != sessit.end(); ++it)
			{
				pair<string,string> session(*it,*it);
				sessions.push_back(session);
			}
		}
	}
}


pair<string,string> Cfg::nextSession()
{
	currentSession = (currentSession + 1) % sessions.size();
	return sessions[currentSession];
}


/*
 * Choose a theme at random from the list in the config file. IF the theme
 * file cannot be found then issue a warning and try again.
 */
string Cfg::findValidRandomTheme ( const string& set )
{
	/* extract random theme from theme set; return empty string on error */
	string name = set;
	struct stat buf;

	if (name[name.length()-1] == ',')
	{
		name.erase(name.length() - 1);
	}

	Util::srandom(Util::makeseed());

	vector<string> themes;
	string themefile;
	Cfg::split(themes, name, ',');
	do {
		int sel = Util::random() % themes.size();

		name = Cfg::Trim(themes[sel]);
		themefile = string(THEMESDIR) +"/" + name + THEMESFILE;
		if (stat(themefile.c_str(), &buf) != 0)
		{
			themes.erase(find(themes.begin(), themes.end(), name));
			logStream << APPNAME << ": Invalid theme in config: "
				 << name << endl;
			name = "";
		}
	} while (name == "" && themes.size());
	return name;
}

