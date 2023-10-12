/*
 * SLiM - Simple Login Manager
 *  Copyright (C) 1997, 1998 Per Liden
 *  Copyright (C) 2004-06 Simone Rota <sip@varlock.com>
 *  Copyright (C) 2004-06 Johannes Winkelmann <jw@tks6.net>
 *  Copyright (C) 2022-23 Rob Pearce <slim@flitspace.org.uk>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#include <sstream>
#include <poll.h>
#include <libgen.h>
#include <X11/extensions/Xrandr.h>
#include <X11/Xatom.h>		// for XA_PIXMAP
#include <unistd.h>			// for sleep
#include "const.h"
#include "image.h"
#include "log.h"
#include "cfg.h"
#include "switchuser.h"
#include "panel.h"

using namespace std;

Panel::Panel(Display* dpy, int scr, Window root, Cfg* config,
			 const string& themedir, PanelType panel_mode)
	: cfg(config), mode(panel_mode), Dpy(dpy), Scr(scr), Root(root),
	  session_name(""), session_exec("")
{
	if (mode == Mode_Lock)
	{
		Win = root;
		viewport = GetPrimaryViewport();
	}
	else if ( mode == Mode_Test )
	{
		XWindowAttributes attributes;
		XGetWindowAttributes(Dpy, Root, &attributes);
		viewport.x      = 0;	// Not actually used. The window's position
		viewport.y      = 0;	// is irrelevant to our drawing functions
		viewport.width  = attributes.width;
		viewport.height = attributes.height;
	}
	else
	{
		/* The existing behaviour in DM mode was to always use the full 
		 * screen size */
		viewport.x = 0;
		viewport.y = 0;
		// AFAICT the following two lines, used in GetPrimaryViewport, do
		// exactly the same as the ones that follow, which were used elsewhere
		//viewport.width = DisplayWidth(Dpy, Scr);
		//viewport.height = DisplayHeight(Dpy, Scr);
		viewport.width = XWidthOfScreen(ScreenOfDisplay(Dpy, Scr));
		viewport.height = XHeightOfScreen(ScreenOfDisplay(Dpy, Scr));
	}

	/* Init GC */
	XGCValues gcv;
	unsigned long gcm;
	gcm = GCForeground|GCBackground|GCGraphicsExposures;
	gcv.foreground = GetColor("black");
	gcv.background = GetColor("white");
	gcv.graphics_exposures = False;

	TextGC = XCreateGC(Dpy, Root, gcm, &gcv);

	if (mode == Mode_Lock) {
		gcm = GCGraphicsExposures;
		gcv.graphics_exposures = False;
		WinGC = XCreateGC(Dpy, Win, gcm, &gcv);
		if (WinGC == 0) {
			cerr << APPNAME << ": failed to create pixmap\n.";
			exit(ERR_EXIT);
		}
	}

	// Intern _XROOTPMAP_ID property  -  does this belong here?
	BackgroundPixmapId = XInternAtom(Dpy, "_XROOTPMAP_ID", False);

	font = XftFontOpenName(Dpy, Scr, cfg->getOption("input_font").c_str());
	welcomefont = XftFontOpenName(Dpy, Scr, cfg->getOption("welcome_font").c_str());
	enterfont = XftFontOpenName(Dpy, Scr, cfg->getOption("username_font").c_str());
	msgfont = XftFontOpenName(Dpy, Scr, cfg->getOption("msg_font").c_str());

	Visual* visual = DefaultVisual(Dpy, Scr);
	Colormap colormap = DefaultColormap(Dpy, Scr);
	/* NOTE: using XftColorAllocValue() would be a better solution. Lazy me. */
	XftColorAllocName(Dpy, visual, colormap, cfg->getOption("input_color").c_str(), &inputcolor);
	XftColorAllocName(Dpy, visual, colormap, cfg->getOption("input_shadow_color").c_str(), &inputshadowcolor);
	XftColorAllocName(Dpy, visual, colormap, cfg->getOption("welcome_color").c_str(), &welcomecolor);
	XftColorAllocName(Dpy, visual, colormap, cfg->getOption("welcome_shadow_color").c_str(), &welcomeshadowcolor);
	XftColorAllocName(Dpy, visual, colormap, cfg->getOption("username_color").c_str(), &entercolor);
	XftColorAllocName(Dpy, visual, colormap, cfg->getOption("username_shadow_color").c_str(), &entershadowcolor);
	XftColorAllocName(Dpy, visual, colormap, cfg->getOption("msg_color").c_str(), &msgcolor);
	XftColorAllocName(Dpy, visual, colormap, cfg->getOption("msg_shadow_color").c_str(), &msgshadowcolor);
	XftColorAllocName(Dpy, visual, colormap,
					  cfg->getOption("session_color").c_str(), &sessioncolor);
	XftColorAllocName(Dpy, visual, colormap,
					  cfg->getOption("session_shadow_color").c_str(), &sessionshadowcolor);

	/* Load properties from config / theme */
	input_name_x = cfg->getIntOption("input_name_x");
	input_name_y = cfg->getIntOption("input_name_y");
	input_pass_x = cfg->getIntOption("input_pass_x");
	input_pass_y = cfg->getIntOption("input_pass_y");
	inputShadowXOffset = cfg->getIntOption("input_shadow_xoffset");
	inputShadowYOffset = cfg->getIntOption("input_shadow_yoffset");

	if (input_pass_x < 0 || input_pass_y < 0){ /* single inputbox mode */
		input_pass_x = input_name_x;
		input_pass_y = input_name_y;
	}

	/* Load panel and background image */
	string panelpng = "";
	panelpng = panelpng + themedir +"/panel.png";
	image = new Image;
	bool loaded = image->Read(panelpng.c_str());
	if (!loaded) { /* try jpeg if png failed */
		panelpng = themedir + "/panel.jpg";
		loaded = image->Read(panelpng.c_str());
		if (!loaded) {
			logStream << APPNAME
				 << ": could not load panel image for theme '"
				 << basename((char*)themedir.c_str()) << "'"
				 << endl;
			exit(ERR_EXIT);
		}
	}

	bgImg = new Image();
	string bgstyle = cfg->getOption("background_style");
	if (bgstyle != "color") {
		panelpng = themedir +"/background.png";
		loaded = bgImg->Read(panelpng.c_str());
		if (!loaded) { /* try jpeg if png failed */
			panelpng = themedir + "/background.jpg";
			loaded = bgImg->Read(panelpng.c_str());
			if (!loaded){
				logStream << APPNAME
					 << ": could not load background image for theme '"
					 << basename((char*)themedir.c_str()) << "'"
					 << endl;
				exit(ERR_EXIT);
			}
		}
	}

	if (bgstyle == "stretch")
		bgImg->Resize(viewport.width, viewport.height);
	else if (bgstyle == "tile")
		bgImg->Tile(viewport.width, viewport.height);
	else if (bgstyle == "center") {
		string hexvalue = cfg->getOption("background_color");
		hexvalue = hexvalue.substr(1,6);
		bgImg->Center(viewport.width,
			viewport.height,
			hexvalue.c_str());
	} else { // plain color or error
		string hexvalue = cfg->getOption("background_color");
		hexvalue = hexvalue.substr(1,6);
		bgImg->Center(viewport.width,
			viewport.height,
			hexvalue.c_str());
	}

	string cfgX = cfg->getOption("input_panel_x");
	string cfgY = cfg->getOption("input_panel_y");

	X = Cfg::absolutepos(cfgX, viewport.width, image->Width());
	Y = Cfg::absolutepos(cfgY, viewport.height, image->Height());

	if (mode == Mode_Lock) {
		// In slimlock, the window we draw on is always the root, not the 
		// XSimpleWindow created by OpenPanel to represent the panel area.
		// Thus we apply a hack to offset the input positions. It's a shame
		// we don't do the job properly, as this hack isn't applied to any
		// of the static text
		input_name_x += X;
		input_name_y += Y;
		input_pass_x += X;
		input_pass_y += Y;
	}

	if (mode == Mode_Lock) {
		/* Merge image into background without crop, so that PanelPixmap is
		 * the whole screen (background image with panel drawn on it) */
		image->Merge_non_crop(bgImg, X, Y);
	} else {
		/* Merge image with cropped background, so that PanelPixmap is the
		 * panel with the relevant part of the X root image included instead
		 * of the alpha channel */
		image->Merge(bgImg, X, Y);
	}
	PanelPixmap = image->createPixmap(Dpy, Scr, Root);

	/* Read (and substitute vars in) the welcome message */
	welcome_message = cfg->getWelcomeMessage();

	if (mode == Mode_Lock) {
		SetName(getenv("USER"));
		field = Get_Passwd;
		OnExpose();
	}
	MsgExtents.width = 0;
}

Panel::~Panel()
{
	Visual* visual = DefaultVisual(Dpy, Scr);
	Colormap colormap = DefaultColormap(Dpy, Scr);

	XftColorFree(Dpy, visual, colormap, &inputcolor);
	XftColorFree(Dpy, visual, colormap, &inputshadowcolor);
	XftColorFree(Dpy, visual, colormap, &welcomecolor);
	XftColorFree(Dpy, visual, colormap, &welcomeshadowcolor);
	XftColorFree(Dpy, visual, colormap, &entercolor);
	XftColorFree(Dpy, visual, colormap, &entershadowcolor);
	XftColorFree(Dpy, visual, colormap, &msgcolor);
	XftColorFree(Dpy, visual, colormap, &msgshadowcolor);
	XftColorFree(Dpy, visual, colormap, &sessioncolor);
	XftColorFree(Dpy, visual, colormap, &sessionshadowcolor);

	XFreeGC(Dpy, TextGC);
	XftFontClose(Dpy, font);
	XftFontClose(Dpy, msgfont);
	XftFontClose(Dpy, welcomefont);
	XftFontClose(Dpy, enterfont);

	if (mode == Mode_Lock)
		XFreeGC(Dpy, WinGC);

	delete bgImg;
	delete image;
}


/**
 * Set the (previously loaded and adjusted) background image as the window
 * background for the root window.
 */
void Panel::setBackground(void)
{
	Pixmap p = bgImg->createPixmap(Dpy, Scr, Root);
	XSetWindowBackgroundPixmap(Dpy, Root, p);
	XChangeProperty(Dpy, Root, BackgroundPixmapId, XA_PIXMAP, 32,
				PropModeReplace, (unsigned char *)&p, 1);

	XClearWindow(Dpy, Root);

	XFlush(Dpy);
}


/* Hide the cursor */
void Panel::HideCursor()
{
	if (cfg->getOption("hidecursor") == "true")
	{
		XColor	black;
		char	cursordata[1];
		Pixmap	cursorpixmap;
		Cursor	cursor;
		cursordata[0]=0;
		cursorpixmap = XCreateBitmapFromData(Dpy, Root, cursordata, 1, 1);
		black.red=0;
		black.green=0;
		black.blue=0;
		cursor = XCreatePixmapCursor(Dpy, cursorpixmap, cursorpixmap, &black, &black, 0, 0);
		//XFreePixmap(dpy, cursorpixmap);		// man page is confusing as to whether this is right
		XDefineCursor(Dpy, Root, cursor);
	}
}


/* Open the login panel. Not used by slimlock */
void Panel::OpenPanel()
{
	/* Create window */
	Win = XCreateSimpleWindow(Dpy, Root, X, Y,
							  image->Width(),
							  image->Height(),
							  0, GetColor("white"), GetColor("white"));

	/* Events */
	XSelectInput(Dpy, Win, ExposureMask | KeyPressMask);

	/* Set background */
	XSetWindowBackgroundPixmap(Dpy, Win, PanelPixmap);

	/* Show window */
	XMapWindow(Dpy, Win);
	XMoveWindow(Dpy, Win, X, Y); /* override wm positioning (for tests) */

	/* Grab keyboard */
	XGrabKeyboard(Dpy, Win, False, GrabModeAsync, GrabModeAsync, CurrentTime);

	XFlush(Dpy);
}

void Panel::ClosePanel()
{
	XUngrabKeyboard(Dpy, CurrentTime);
	XUnmapWindow(Dpy, Win);
	XDestroyWindow(Dpy, Win);
	XFlush(Dpy);
}

void Panel::WrongPassword(int timeout)
{
	string message;

	if ( mode != Mode_Lock )
	{
		XClearWindow(Dpy, Root);
	}

#if 0
	if (CapsLockOn)
		message = cfg->getOption("passwd_feedback_capslock");
	else
#endif
	message = cfg->getOption("passwd_feedback_msg");

	XftDraw *draw = XftDrawCreate ( Dpy, Root,
		DefaultVisual(Dpy, Scr), DefaultColormap(Dpy, Scr) );
	XftTextExtents8(Dpy, msgfont, reinterpret_cast<const XftChar8*>(message.c_str()),
		message.length(), &MsgExtents);

	string cfgX = cfg->getOption("passwd_feedback_x");
	string cfgY = cfg->getOption("passwd_feedback_y");
	int shadowXOffset = cfg->getIntOption("msg_shadow_xoffset");
	int shadowYOffset = cfg->getIntOption("msg_shadow_yoffset");
	int msg_x = Cfg::absolutepos(cfgX, viewport.width, MsgExtents.width);
	int msg_y = Cfg::absolutepos(cfgY, viewport.height, MsgExtents.height);

	MsgExtents.x = msg_x;
	MsgExtents.y = msg_y - MsgExtents.height;

	if ( timeout > 0 )
	{
		OnExpose();
		if ( msg_x >= 0 && msg_y >= 0 )
			SlimDrawString8(draw, &msgcolor, msgfont, msg_x, msg_y, message,
					&msgshadowcolor, shadowXOffset, shadowYOffset);

		if (cfg->getOption("bell") == "1")
			XBell(Dpy, 100);
		XFlush(Dpy);
		sleep(timeout);
	}
	ResetPasswd();
	if ( mode != Mode_Lock )
	{
		if ( cfg->getIntOption("keep_user_on_fail") == 0 )
		{
			ResetName();
		}
		field = Get_Name;
	}
	OnExpose();
	// The message should stay on the screen even after the password field is
	// cleared, methinks. I don't like this solution, but it works.
	if ( msg_x >= 0 && msg_y >= 0 )
		SlimDrawString8(draw, &msgcolor, msgfont, msg_x, msg_y, message,
				&msgshadowcolor, shadowXOffset, shadowYOffset);
	XSync(Dpy, True);
	XftDrawDestroy(draw);
}

void Panel::Message(const string& text)
{
	string cfgX, cfgY;
	XGlyphInfo extents;
	XftDraw *draw;

	// The message positions are screen-relative, not panel-relative
	draw = XftDrawCreate(Dpy, Root,
		DefaultVisual(Dpy, Scr), DefaultColormap(Dpy, Scr));

	XftTextExtents8(Dpy, msgfont,
		reinterpret_cast<const XftChar8*>(text.c_str()),
					text.length(), &extents);
	cfgX = cfg->getOption("msg_x");
	cfgY = cfg->getOption("msg_y");
	int shadowXOffset = cfg->getIntOption("msg_shadow_xoffset");
	int shadowYOffset = cfg->getIntOption("msg_shadow_yoffset");
	int msg_x, msg_y;

	msg_x = Cfg::absolutepos(cfgX, viewport.width, extents.width);
	msg_y = Cfg::absolutepos(cfgY, viewport.height, extents.height);

	SlimDrawString8 (draw, &msgcolor, msgfont, msg_x, msg_y,
					 text,
					 &msgshadowcolor,
					 shadowXOffset, shadowYOffset);
	XFlush(Dpy);
	XftDrawDestroy(draw);
}

unsigned long Panel::GetColor(const char* colorname)
{
	XColor color;
	XWindowAttributes attributes;

	XGetWindowAttributes(Dpy, Root, &attributes);

	color.pixel = 0;

	if(!XParseColor(Dpy, attributes.colormap, colorname, &color))
		logStream << APPNAME << ": can't parse color " << colorname << endl;
	else if(!XAllocColor(Dpy, attributes.colormap, &color))
		logStream << APPNAME << ": can't allocate color " << colorname << endl;

	return color.pixel;
}

void Panel::TextCursor(int visible)
{
	const char* text = NULL;
	int xx = 0, yy = 0, y2 = 0, cheight = 0;
	const char* txth = "Wj"; /* used to get cursor height */

	// The constructor and other conditionals guarantee that
	// if (mode == Mode_Lock)	field = Get_Passwd;

	switch(field) {
		case Get_Passwd:
			text = HiddenPasswdBuffer.c_str();
			xx = input_pass_x;
			yy = input_pass_y;
			break;

		case Get_Name:
			text = NameBuffer.c_str();
			xx = input_name_x;
			yy = input_name_y;
			break;
	}

	XGlyphInfo extents;
	XftTextExtents8(Dpy, font, (XftChar8*)txth, strlen(txth), &extents);
	cheight = extents.height;
	y2 = yy - extents.y + extents.height;
	XftTextExtents8(Dpy, font, (XftChar8*)text, strlen(text), &extents);
	xx += extents.width;

	if(visible == SHOW) {
		if (mode == Mode_Lock) {
			xx += viewport.x;
			yy += viewport.y;
			y2 += viewport.y;
		}
		XSetForeground(Dpy, TextGC,
			GetColor(cfg->getOption("input_color").c_str()));

		XDrawLine(Dpy, Win, TextGC,
				  xx+1, yy-cheight,
				  xx+1, y2);
	} else {
		if (mode == Mode_Lock)
			ApplyBackground(Rectangle(xx+1, yy-cheight,
				1, y2-(yy-cheight)+1));
		else
			XClearArea(Dpy, Win, xx+1, yy-cheight,
				1, y2-(yy-cheight)+1, false);
	}
}

void Panel::EventHandler(const Panel::FieldType& curfield)
{
	XEvent event;
	field = curfield;
	bool loop = true;

	if ( (mode != Mode_Lock) && ( MsgExtents.width == 0 ) )
		OnExpose();

	struct pollfd x11_pfd = {0};
	x11_pfd.fd = ConnectionNumber(Dpy);
	x11_pfd.events = POLLIN;

	while (loop)
	{
		if (XPending(Dpy) || poll(&x11_pfd, 1, -1) > 0)
		{
			while(XPending(Dpy))
			{
				XNextEvent(Dpy, &event);
				switch(event.type)
				{
					case Expose:
						OnExpose();
						break;

					case KeyPress:
						loop=OnKeyPress(event);
						break;
				}
			}
			if ( MsgExtents.width > 0 )
			{
				if (mode == Mode_Lock)
					ApplyBackground(Rectangle(MsgExtents.x,
						MsgExtents.y, MsgExtents.width+1,
						MsgExtents.height+2));
				else
					XClearArea(Dpy, Root, MsgExtents.x, MsgExtents.y,
						MsgExtents.width+1, MsgExtents.height+2, false);
				MsgExtents.width = 0;
			}
		}
	}

	return;
}

void Panel::OnExpose(void)
{
	XftDraw *draw = XftDrawCreate(Dpy, Win,
		DefaultVisual(Dpy, Scr), DefaultColormap(Dpy, Scr));

	if (mode == Mode_Lock)
		ApplyBackground();
	else
		XClearWindow(Dpy, Win);

	if (input_pass_x != input_name_x || input_pass_y != input_name_y){
		SlimDrawString8 (draw, &inputcolor, font, input_name_x, input_name_y,
						 NameBuffer,
						 &inputshadowcolor,
						 inputShadowXOffset, inputShadowYOffset);
		SlimDrawString8 (draw, &inputcolor, font, input_pass_x, input_pass_y,
						 HiddenPasswdBuffer,
						 &inputshadowcolor,
						 inputShadowXOffset, inputShadowYOffset);
	} else { /*single input mode */
		switch(field) {
			case Get_Passwd:
				SlimDrawString8 (draw, &inputcolor, font,
								 input_pass_x, input_pass_y,
								 HiddenPasswdBuffer,
								 &inputshadowcolor,
								 inputShadowXOffset, inputShadowYOffset);
				break;
			case Get_Name:
				SlimDrawString8 (draw, &inputcolor, font,
								 input_name_x, input_name_y,
								 NameBuffer,
								 &inputshadowcolor,
								 inputShadowXOffset, inputShadowYOffset);
				break;
		}
	}

	XftDrawDestroy (draw);
	TextCursor(SHOW);
	ShowText();
}

void Panel::EraseLastChar(std::string &formerString)
{
	switch(field) {
	case Get_Name:
		if (! NameBuffer.empty()) {
			formerString=NameBuffer;
			NameBuffer.erase(--NameBuffer.end());
		}
		break;

	case Get_Passwd:
		if (!PasswdBuffer.empty()) {
			formerString=HiddenPasswdBuffer;
			PasswdBuffer.erase(--PasswdBuffer.end());
			HiddenPasswdBuffer.erase(--HiddenPasswdBuffer.end());
		}
		break;
	}
}

bool Panel::OnKeyPress(XEvent& event)
{
	char ascii;
	KeySym keysym;
	XComposeStatus compstatus;
	int xx = 0;
	int yy = 0;
	string text;
	string formerString = "";

	XLookupString(&event.xkey, &ascii, 1, &keysym, &compstatus);
	switch(keysym){
		case XK_F1:
			if ( mode != Mode_Lock )	// Can't change session in a screen lock
				SwitchSession();
			return true;

		case XK_F11:
			/* Take a screenshot */
			if ( system(cfg->getOption("screenshot_cmd").c_str()) < 0 )
				logStream << APPNAME << ": screenshot_cmd failed" << endl;
			return true;

		case XK_Return:
		case XK_KP_Enter:
			if (field==Get_Name)
			{
				/* Don't allow an empty username */
				if (NameBuffer.empty())
					return true;

				if (NameBuffer==CONSOLE_STR)
					action = Console;
				else if (NameBuffer==HALT_STR)
					action = Halt;
				else if (NameBuffer==REBOOT_STR)
					action = Reboot;
				else if (NameBuffer==SUSPEND_STR)
					action = Suspend;
				else if (NameBuffer==EXIT_STR)
					action = Exit;
				else
					action = Login;
			}
			return false;

		default:
			break;
	}

	TextCursor(HIDE);
	switch(keysym){
		case XK_Delete:
		case XK_BackSpace:
			EraseLastChar(formerString);
			break;

		case XK_w:
		case XK_u:
			if (reinterpret_cast<XKeyEvent&>(event).state & ControlMask)
			{
				switch(field) {
					case Get_Passwd:
						formerString = HiddenPasswdBuffer;
						HiddenPasswdBuffer.clear();
						PasswdBuffer.clear();
						break;
					case Get_Name:
						formerString = NameBuffer;
						NameBuffer.clear();
						break;
				}
				break;
			}
			/* Deliberate fall-through */
		case XK_h:
			if (reinterpret_cast<XKeyEvent&>(event).state & ControlMask)
			{
				EraseLastChar(formerString);
				break;
			}
			/* Deliberate fall-through */
		default:
			if (isprint(ascii) && (keysym < XK_Shift_L || keysym > XK_Hyper_R))
			{
				switch(field) {
					case Get_Name:
						formerString=NameBuffer;
						if (NameBuffer.length() < INPUT_MAXLENGTH_NAME-1)
						{
							NameBuffer.append(&ascii,1);
						}
						break;
					case Get_Passwd:
						formerString=HiddenPasswdBuffer;
						if (PasswdBuffer.length() < INPUT_MAXLENGTH_PASSWD-1)
						{
							PasswdBuffer.append(&ascii,1);
							HiddenPasswdBuffer.append("*");
						}
					break;
				}
			}
			else
			{	// *RP* I think this is to fix the fake bolding if the user presses TAB
				return true; //nodraw if notchange
			}
			break;
	}

	XGlyphInfo extents;
	XftDraw *draw = XftDrawCreate(Dpy, Win,
			  DefaultVisual(Dpy, Scr), DefaultColormap(Dpy, Scr));

	switch(field) {
		case Get_Name:
			text = NameBuffer;
			xx = input_name_x;
			yy = input_name_y;
			break;

		case Get_Passwd:
			text = HiddenPasswdBuffer;
			xx = input_pass_x;
			yy = input_pass_y;
			break;
	}

	if (!formerString.empty())
	{
		const char* txth = "Wj"; /* get proper maximum height ? */
		XftTextExtents8(Dpy, font,
				reinterpret_cast<const XftChar8*>(txth), strlen(txth), &extents);
		int maxHeight = extents.height;

		XftTextExtents8(Dpy, font,
				reinterpret_cast<const XftChar8*>(formerString.c_str()),
						formerString.length(), &extents);
		int maxLength = extents.width;

		if (mode == Mode_Lock)
			ApplyBackground(Rectangle(input_pass_x - 3,
				input_pass_y - maxHeight - 3,
				maxLength + 6, maxHeight + 6));
		else
			XClearArea(Dpy, Win, xx - 3, yy-maxHeight - 3,
				maxLength + 6, maxHeight + 6, false);
	}

	if (!text.empty())
	{
		SlimDrawString8 (draw, &inputcolor, font, xx, yy,
				 text,
				 &inputshadowcolor,
				 inputShadowXOffset, inputShadowYOffset);
	}

	XftDrawDestroy (draw);
	TextCursor(SHOW);
	return true;
}


/* Draw welcome and "enter username" messages */
void Panel::ShowText()
{
	string cfgX, cfgY;
	XGlyphInfo extents;

	bool singleInputMode = ( input_name_x == input_pass_x
			    && input_name_y == input_pass_y );

	/// @bug this draw context is assumed relative to the panel but in lock
	///		 mode it's actually relative to the background
	XftDraw *draw = XftDrawCreate(Dpy, Win,
		  DefaultVisual(Dpy, Scr), DefaultColormap(Dpy, Scr));
	/* welcome message */
	XftTextExtents8(Dpy, welcomefont, (XftChar8*)welcome_message.c_str(),
					strlen(welcome_message.c_str()), &extents);
	cfgX = cfg->getOption("welcome_x");
	cfgY = cfg->getOption("welcome_y");
	int shadowXOffset = cfg->getIntOption("welcome_shadow_xoffset");
	int shadowYOffset = cfg->getIntOption("welcome_shadow_yoffset");

	welcome_x = Cfg::absolutepos(cfgX, image->Width(), extents.width);
	welcome_y = Cfg::absolutepos(cfgY, image->Height(), extents.height);
	if (welcome_x >= 0 && welcome_y >= 0) {
		SlimDrawString8 (draw, &welcomecolor, welcomefont,
						 welcome_x, welcome_y,
						 welcome_message,
						 &welcomeshadowcolor, shadowXOffset, shadowYOffset);
	}

	/* Enter username-password message */
	string msg;
	if (!singleInputMode|| field == Get_Passwd) {
		msg = cfg->getOption("password_msg");
		XftTextExtents8(Dpy, enterfont, (XftChar8*)msg.c_str(),
						strlen(msg.c_str()), &extents);
		cfgX = cfg->getOption("password_x");
		cfgY = cfg->getOption("password_y");
		int shadowXOffset = cfg->getIntOption("username_shadow_xoffset");
		int shadowYOffset = cfg->getIntOption("username_shadow_yoffset");
		password_x = Cfg::absolutepos(cfgX, image->Width(), extents.width);
		password_y = Cfg::absolutepos(cfgY, image->Height(), extents.height);
		if (password_x >= 0 && password_y >= 0){
			SlimDrawString8 (draw, &entercolor, enterfont, password_x, password_y,
							 msg, &entershadowcolor, shadowXOffset, shadowYOffset);
		}
	}

	if ((!singleInputMode|| field == Get_Name) && mode != Mode_Lock ) {
		msg = cfg->getOption("username_msg");
		XftTextExtents8(Dpy, enterfont, (XftChar8*)msg.c_str(),
						strlen(msg.c_str()), &extents);
		cfgX = cfg->getOption("username_x");
		cfgY = cfg->getOption("username_y");
		int shadowXOffset = cfg->getIntOption("username_shadow_xoffset");
		int shadowYOffset = cfg->getIntOption("username_shadow_yoffset");
		username_x = Cfg::absolutepos(cfgX, image->Width(), extents.width);
		username_y = Cfg::absolutepos(cfgY, image->Height(), extents.height);
		if (username_x >= 0 && username_y >= 0){
			SlimDrawString8 (draw, &entercolor, enterfont, username_x, username_y,
							 msg, &entershadowcolor, shadowXOffset, shadowYOffset);
		}
	}
	XftDrawDestroy(draw);

	if (mode == Mode_Lock) {
		// If only the password box is visible, draw the user name somewhere too
		string user_msg = "User: " + GetName();
		int show_username = cfg->getIntOption("show_username");
		if (singleInputMode && show_username) {
			Message(user_msg);
		}
	}
}


string Panel::getSession()
{
	return session_exec;
}


/* choose next available session type. Not used in lock mode */
void Panel::SwitchSession()
{
	pair<string,string> ses = cfg->nextSession();
	session_name = ses.first;
	session_exec = ses.second;
	if (session_name.size() > 0) {
		ShowSession();
	}
 }


/* Display session type on the screen. Not used in lock mode */
void Panel::ShowSession()
{
	string msg_x, msg_y;
	XClearWindow(Dpy, Root);
	string currsession = cfg->getOption("session_msg") + " " + session_name;
	XGlyphInfo extents;

	sessionfont = XftFontOpenName(Dpy, Scr, cfg->getOption("session_font").c_str());

	XftDraw *draw = XftDrawCreate(Dpy, Root,
								  DefaultVisual(Dpy, Scr), DefaultColormap(Dpy, Scr));
	XftTextExtents8(Dpy, sessionfont, reinterpret_cast<const XftChar8*>(currsession.c_str()),
					currsession.length(), &extents);
	msg_x = cfg->getOption("session_x");
	msg_y = cfg->getOption("session_y");
	int x = Cfg::absolutepos(msg_x, viewport.width, extents.width);
	int y = Cfg::absolutepos(msg_y, viewport.height, extents.height);
	int shadowXOffset = cfg->getIntOption("session_shadow_xoffset");
	int shadowYOffset = cfg->getIntOption("session_shadow_yoffset");

	SlimDrawString8(draw, &sessioncolor, sessionfont, x, y,
					currsession,
					&sessionshadowcolor,
					shadowXOffset, shadowYOffset);
	XFlush(Dpy);
	XftDrawDestroy(draw);
}


void Panel::SlimDrawString8(XftDraw *d, XftColor *color, XftFont *font,
							int x, int y, const string& str,
							XftColor* shadowColor,
							int xOffset, int yOffset)
{
	if (mode == Mode_Lock) {
		x += viewport.x;
		y += viewport.y;
	}

	if (xOffset && yOffset) {
		XftDrawStringUtf8(d, shadowColor, font, x+xOffset, y+yOffset,
			reinterpret_cast<const FcChar8*>(str.c_str()), str.length());
	}

	XftDrawStringUtf8(d, color, font, x, y,
		reinterpret_cast<const FcChar8*>(str.c_str()), str.length());
}

Panel::ActionType Panel::getAction(void) const
{
	return action;
}

void Panel::Reset(void)
{
	ResetName();
	ResetPasswd();
}

void Panel::ResetName(void)
{
	NameBuffer.clear();
}

void Panel::ResetPasswd(void)
{
	PasswdBuffer.clear();
	HiddenPasswdBuffer.clear();
}


/* Pre-load the user name input box with the provided string.
 * Used to set the default user, if so configured, and by slimlock to set the
 * currently logged-in user.
 */
void Panel::SetName(const string& name)
{
	NameBuffer=name;
	action = Login;
}

const string& Panel::GetName(void) const
{
	return NameBuffer;
}

const string& Panel::GetPasswd(void) const
{
	return PasswdBuffer;
}


/**
 * Identify the viewport (physical screen?) to draw on. This allows slimlock
 * to handle Xinerama-type multi-monitor setups. Not currently used by slim
 */
Rectangle Panel::GetPrimaryViewport()
{
	Rectangle fallback;
	Rectangle result;

	RROutput primary;
	XRROutputInfo *primary_info;
	XRRScreenResources *resources;
	XRRCrtcInfo *crtc_info;

    int crtc;

	fallback.x = 0;
	fallback.y = 0;
	fallback.width = DisplayWidth(Dpy, Scr);
	fallback.height = DisplayHeight(Dpy, Scr);

	resources = XRRGetScreenResources(Dpy, Win);
	if (!resources)
	{
		cerr << "XRRGetScreenResources failed\n";
	    return fallback;
	}

	primary = XRRGetOutputPrimary(Dpy, Win);
	if (!primary) {
	    // No "primary" defined (by the WM, usually) but could still have
	    // multiple monitors or setups, so default to the first output.
	    primary = resources->outputs[0];
	}

	primary_info = XRRGetOutputInfo(Dpy, resources, primary);
	if (!primary_info) {
		cerr << "XRRGetOutputInfo failed\n";
	    XRRFreeScreenResources(resources);
	    return fallback;
	}

    // Fixes bug with multiple monitors.  Just pick first monitor if 
    // XRRGetOutputInfo gives returns bad value for crtc.
    if (primary_info->crtc < 1) {
        if (primary_info->ncrtc > 0) {
           crtc = primary_info->crtcs[0];
        } else {
            cerr << "Cannot get crtc from xrandr.\n";
            exit(EXIT_FAILURE);
        }
    } else {
        crtc = primary_info->crtc;
    }

	crtc_info = XRRGetCrtcInfo(Dpy, resources, crtc);

	if (!crtc_info) {
		cerr << "XRRGetCrtcInfo failed\n";
	    XRRFreeOutputInfo(primary_info);
	    XRRFreeScreenResources(resources);
	    return fallback;
	}

	result.x = crtc_info->x;
	result.y = crtc_info->y;
	result.width = crtc_info->width;
	result.height = crtc_info->height;

	XRRFreeCrtcInfo(crtc_info);
	XRRFreeOutputInfo(primary_info);
	XRRFreeScreenResources(resources);

	return result;
}


/**
 * Re-draw the background over a rectangle. This method is only used in "lock"
 * mode - the DM mode uses XClearArea instead.
 */
void Panel::ApplyBackground(Rectangle rect)
{
	int ret = 0;

	if (rect.is_empty()) {
	    rect.x = 0;
	    rect.y = 0;
	    rect.width = viewport.width;
	    rect.height = viewport.height;
	}

	ret = XCopyArea(Dpy, PanelPixmap, Win, WinGC,
		rect.x, rect.y, rect.width, rect.height,
		viewport.x + rect.x, viewport.y + rect.y);

	if (!ret)
	    cerr << APPNAME << ": failed to put pixmap on the screen\n.";
}

