/* SLiM - Simple Login Manager
	Copyright (C) 2004-06 Simone Rota <sip@varlock.com>
	Copyright (C) 2004-06 Johannes Winkelmann <jw@tks6.net>
	Copyright (C) 2012		Nobuhiro Iwamatsu <iwamatsu@nigauri.org>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
*/

#ifndef _NUMLOCK_H_
#define _NUMLOCK_H_

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

class NumLock
{
public:
	NumLock();
	static void setOn(Display *dpy);
	static void setOff(Display *dpy);

private:
	static int xkb_init(Display *dpy);
	static unsigned int xkb_mask_modifier(XkbDescPtr xkb, const char *name);
	static unsigned int xkb_numlock_mask(Display *dpy);
	static void control_numlock(Display *dpy, bool flag);
};

#endif /* _NUMLOCK_H_ */
