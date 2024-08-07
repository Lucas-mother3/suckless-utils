/* See LICENSE file for copyright and license details. */

/* appearance */
static char font[]        = "Hack Nerd Font:size=12";
static char* normbgcolor  = "#222222";
static char* normfgcolor  = "#cccccc";
static char* selbgcolor   = "#555555";
static char* selfgcolor   = "#ffffff";
static char* urgbgcolor   = "#111111";
static char* urgfgcolor   = "#cc0000";
static char before[]      = "<";
static char after[]       = ">";
static char titletrim[]   = "...";
static int  tabwidth      = 200;
static int  foreground    = 1;
static int  urgentswitch  = 0;
#if SEPARATOR_PATCH
static int  separator     = 4;
#endif // SEPARATOR_PATCH

#if BAR_HEIGHT_PATCH
static const int barheight = 0;  /* 0 means derive by font (default), otherwise absolute height */
#endif // BAR_HEIGHT_PATCH

/*
 * Where to place a new tab when it is opened. When npisrelative is True,
 * then the current position is changed + newposition. If npisrelative
 * is False, then newposition is an absolute position.
 */
static int  newposition   = 0;
static Bool npisrelative  = False;

#define SETPROP(p) { \
        .v = (char *[]){ "/bin/sh", "-c", \
                "prop=\"$(xwininfo -children -id $1 | grep '^     0x' |" \
                "sed -e's@^ *\\(0x[0-9a-f]*\\) \"\\([^\"]*\\)\".*@\\1 \\2@' |" \
                "tail -n +2 | spmenu -l 10 -g 1 -p 'Switch to: ')\" &&" \
                "xprop -id $1 -f $0 8s -set $0 \"$prop\"", \
                p, winid, NULL \
        } \
}

#if REPARENT_PATCH
/* Modify the following line to match your terminal and software list */
#define OPENTERMSOFT(p) { \
	.v = (char *[]){ "/bin/sh", "-c", \
		"term='st' && titlearg='-t' && embedarg='-w' &&" \
		"softlist=$(printf '%s\n' \"htop\" \"btm\" \"vim\" \"broot\" \"ncmpcpp\" \"cmatrix\" \"cava\" \"pipes.sh\" \"adelle-theme\" \"xplr\" \"ytfzf\" \"newsboat\" \"sfm\" \"vis\" \"sptlrx\" \"nvim\") &&" \
		"printf '%s' \"$softlist\" |" \
		"spmenu -l 10 -g 1 -p 'Softwares to run: ' |" \
		"xargs -I {} $term $titlearg \"{}\" $embedarg $1 -e \"{}\"", \
		p, winid, NULL \
	} \
}


/* Modify the following line to match your terminal*/
#define OPENTERM(p) { \
	.v = (char *[]){ "/bin/sh", "-c", \
		"term='st' && embedarg='-w' &&" \
		"cd \"$(xwininfo -children -id $1 | grep '^     0x' |" \
                "sed -e's@^ *\\(0x[0-9a-f]*\\) \"\\([^\"]*\\)\".*@\\1 \\2@' |" \
		"spmenu -l 10 -g 1 -p 'New term path based on: ' |" \
		"cut -f 1 | xargs -I {} xprop -id \"{}\" | grep _NET_WM_PID |" \
		"cut -d ' ' -f 3 | xargs -I {} pstree -p \"{}\" |" \
		"cut -d '(' -f 3 | cut -d ')' -f 1 |" \
		"xargs -I {} readlink -e /proc/\"{}\"/cwd/)\" &&" \
		"$term $embedarg $1", \
		p, winid, NULL \
	} \
}

/* deskid: id for current workspace */
/* rootid: id for root window */
/* window: data for chosen window by dmenu */
/* wid: chosen window's window id */
/* wname: chosen window's name */
/* cwid: chosen window's child window id (tabbed window only) */
#define ATTACHWIN(p) { \
	.v = (char *[]){ "/bin/sh", "-c", \
		"deskid=$(xdotool get_desktop) &&" \
		"rootid=\"$(xwininfo -root | grep \"Window id\" | cut -d ' ' -f 4)\" &&" \
		"window=\"$(wmctrl -x -l | grep -E \" $deskid \" |" \
		"grep -v $(printf '0x0%x' \"$1\") |" \
		"cut -d ' ' -f 1,4 | spmenu -l 5 -g 1 -p \"Attach: \")\" &&" \
		"wid=$(printf '%s' \"$window\" | cut -d ' ' -f 1) &&" \
		"wname=$(printf '%s' \"$window\" | cut -d ' ' -f 2) &&" \
		"[ \"$wname\" = \"(has no name)\" ] &&" \
		"cwid=$(xwininfo -children -id \"$wid\" | grep '^     0x' |" \
                "sed -e 's@^ *\\(0x[0-9a-f]*\\) \"\\([^\"]*\\)\".*@\\1@') &&" \
		"for id in $(printf '%s' \"$cwid\"); do xdotool windowreparent \"$id\" \"$rootid\"; done &&" \
		"for id in $(printf '%s' \"$cwid\"); do xdotool windowreparent \"$id\" \"$1\"; done ||" \
		"xdotool windowreparent \"$wid\" $1", \
		p, winid, NULL \
	} \
}

#define ATTACHSELECTWIN(p) { \
	.v = (char *[]){ "/bin/sh", "-c", \
		"rootid=\"$(xwininfo -root | grep \"Window id\" | cut -d ' ' -f 4)\" &&" \
		"wid=$(xdotool selectwindow) &&" \
		"wname=$(xwininfo -id \"$wid\" | grep 'Window id:' | cut -d ' ' -f 5-) &&" \
		"[ \"$wname\" = \"(has no name)\" ] &&" \
		"cwid=$(xwininfo -children -id \"$wid\" | grep '^     0x' |" \
                "sed -e 's@^ *\\(0x[0-9a-f]*\\) \"\\([^\"]*\\)\".*@\\1@') &&" \
		"for id in $(printf '%s' \"$cwid\"); do xdotool windowreparent \"$id\" \"$rootid\"; done &&" \
		"for id in $(printf '%s' \"$cwid\"); do xdotool windowreparent \"$id\" \"$1\"; done ||" \
		"xdotool windowreparent \"$wid\" $1", \
		p, winid, NULL \
	} \
}

#define ATTACHALL(p) { \
	.v = (char *[]){ "/bin/sh", "-c", \
		"deskid=$(xdotool get_desktop) &&" \
		"rootid=\"$(xwininfo -root | grep \"Window id\" | cut -d ' ' -f 4)\" &&" \
		"window=\"$(wmctrl -x -l | grep -E \" $deskid \" |" \
		"grep -v $(printf '0x0%x' \"$1\") | cut -d ' ' -f 1,4)\" &&" \
		"IFS=':' &&" \
		"for win in $(printf '%s' \"$window\" | tr '\n' ':'); do unset IFS &&" \
		    "wid=$(printf '%s' \"$win\" | cut -d ' ' -f 1) &&" \
		    "wname=$(printf '%s' \"$win\" | cut -d ' ' -f 2) &&" \
		    "[ \"$wname\" = \"(has no name)\" ] &&" \
		    "{ cwid=$(xwininfo -children -id \"$wid\" | grep '^     0x' |" \
		    "sed -e 's@^ *\\(0x[0-9a-f]*\\) \"\\([^\"]*\\)\".*@\\1@') &&" \
		    "for id in $(printf '%s' \"$cwid\"); do xdotool windowreparent \"$id\" \"$rootid\"; done &&" \
		    "for id in $(printf '%s' \"$cwid\"); do xdotool windowreparent \"$id\" \"$1\"; done; } ||" \
		"xdotool windowreparent \"$wid\" $1; done", \
		p, winid, NULL \
	} \
}

#define STACKTABBED(p) { \
	.v = (char *[]){ "/bin/sh", "-c", \
		"deskid=$(xdotool get_desktop) &&" \
		"rootid=\"$(xwininfo -root | grep \"Window id\" | cut -d ' ' -f 4)\" &&" \
		"window=\"$(wmctrl -x -l | grep -E \" $deskid \" |" \
		"grep -v $(printf '0x0%x' \"$1\") | grep -E tabbed | cut -d ' ' -f 1,4)\" &&" \
		"IFS=':' &&" \
		"for win in $(printf '%s' \"$window\" | tr '\n' ':'); do unset IFS &&" \
		    "wid=$(printf '%s' \"$win\" | cut -d ' ' -f 1) &&" \
		    "wname=$(printf '%s' \"$win\" | cut -d ' ' -f 2) &&" \
		    "[ \"$wname\" = \"(has no name)\" ] &&" \
		    "{ cwid=$(xwininfo -children -id \"$wid\" | grep '^     0x' |" \
		    "sed -e 's@^ *\\(0x[0-9a-f]*\\) \"\\([^\"]*\\)\".*@\\1@') &&" \
		    "for id in $(printf '%s' \"$cwid\"); do xdotool windowreparent \"$id\" \"$rootid\"; done &&" \
		    "for id in $(printf '%s' \"$cwid\"); do xdotool windowreparent \"$id\" \"$1\"; done; } ||" \
		"xdotool windowreparent \"$wid\" $1; done", \
		p, winid, NULL \
	} \
}


#define DETACHWIN(p) { \
        .v = (char *[]){ "/bin/sh", "-c", \
		"rootid=\"$(xwininfo -root | grep \"Window id\" | cut -d ' ' -f 4)\" &&" \
                "wid=\"$(xwininfo -children -id $1 | grep '^     0x' |" \
                "sed -e 's@^ *\\(0x[0-9a-f]*\\) \"\\([^\"]*\\)\".*@\\1 \\2@' |" \
                "spmenu -l 5 -g 1 -p 'Detach: ' | cut -d ' ' -f 1)\" &&" \
		"xwininfo -id $wid -stats | grep -q 'IsUnMapped' && xdotool windowmap $wid;" \
		"xdotool windowreparent \"$wid\" \"$rootid\" &&" \
		"xdotool windowactivate $1", \
                p, winid, NULL \
        } \
}



#define DETACHALL(p) { \
        .v = (char *[]){ "/bin/sh", "-c", \
		"rootid=\"$(xwininfo -root | grep \"Window id\" | cut -d ' ' -f 4)\" &&" \
                "wid=\"$(xwininfo -children -id $1 | grep '^     0x' |" \
                "sed -e 's@^ *\\(0x[0-9a-f]*\\) \"\\([^\"]*\\)\".*@\\1@')\" &&" \
		"IFS=':' &&" \
		"for id in $(printf '%s' \"$wid\" | tr '\n' ':'); do unset IFS &&" \
		    "xdotool windowreparent \"$id\" \"$rootid\" &&" \
		    "xwininfo -id $id -stats |" \
		    "grep -q 'IsUnMapped' &&" \
		    "xdotool windowmap $id; done", \
                p, winid, NULL \
        } \
}



#define SHOWHIDDEN(p) { \
        .v = (char *[]){ "/bin/sh", "-c", \
                "cwin=\"$(xwininfo -children -id $1 | grep '^     0x' |" \
                "sed -e 's@^ *\\(0x[0-9a-f]*\\) \"\\([^\"]*\\)\".*@\\1 \\2@')\" &&" \
		"IFS=':' &&" \
		"for win in $(printf '%s' \"$cwin\" | tr '\n' ':'); do unset IFS &&" \
		    "cwid=$(printf '%s' \"$win\" | cut -d ' ' -f 1) &&" \
		    "xwininfo -id $cwid -stats |" \
		    "grep -q 'IsUnMapped' &&" \
		    "printf '%s\n' \"$win\"; done |" \
		"spmenu -l 5 -g 1 -p \"Show hidden window:\" |" \
		"cut -d ' ' -f 1 |" \
		"xargs -I {} xdotool windowmap \"{}\"", \
                p, winid, NULL \
        } \
}

#define SHOWHIDDENALL(p) { \
        .v = (char *[]){ "/bin/sh", "-c", \
                "cwid=\"$(xwininfo -children -id $1 | grep '^     0x' |" \
                "sed -e 's@^ *\\(0x[0-9a-f]*\\) \"\\([^\"]*\\)\".*@\\1@')\" &&" \
		"IFS=':' &&" \
		"for id in $(printf '%s' \"$cwid\" | tr '\n' ':'); do unset IFS &&" \
		    "xwininfo -id $id -stats | " \
		    "grep -q 'IsUnMapped' &&" \
		    "xdotool windowmap $id; done", \
                p, winid, NULL \
        } \
}

#define HIDEWINDOW(p) { \
        .v = (char *[]){ "/bin/sh", "-c", \
                "cwid=\"$(xwininfo -children -id $1 | grep '^     0x' |" \
                "sed -e 's@^ *\\(0x[0-9a-f]*\\) \"\\([^\"]*\\)\".*@\\1@')\" &&" \
		"IFS=':' && winnum=0 &&" \
		"for id in $(printf '%s' \"$cwid\" | tr '\n' ':'); do unset IFS &&" \
		    "xwininfo -id $id -stats | " \
		    "grep -q 'IsViewable' &&" \
		    "winnum=$(($winnum+1)); done;" \
                "[ $winnum -gt 1 ] &&" \
		"{ xwininfo -children -id $1 | grep '^     0x' | head -n 1 |" \
                "sed -e 's@^ *\\(0x[0-9a-f]*\\) \"\\([^\"]*\\)\".*@\\1@' |" \
		"xargs -I {} xdotool windowunmap \"{}\"; }", \
                p, winid, NULL \
        } \
}

#endif // REPARENT_PATCH

#if XRESOURCES_PATCH
/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
	{ "font",         STRING,  &font },
	{ "color0",       STRING,  &normbgcolor },
	{ "color15",       STRING,  &normfgcolor },
	{ "color6",       STRING,  &selbgcolor },
	{ "color0",       STRING,  &selfgcolor },
	{ "color3",       STRING,  &urgbgcolor },
	{ "color15",       STRING,  &urgfgcolor },
};
#endif // XRESOURCES_PATCH

#define MODKEY Mod4Mask
#if KEYCODE_PATCH
static const Key keys[] = {
	/* modifier             key           function     argument */
	{ MODKEY|ShiftMask,     36,           focusonce,   { 0 } },
	{ MODKEY|ShiftMask,     36,           spawn,       { 0 } },
	{ MODKEY|ShiftMask,     46,           rotate,      { .i = +1 } },
	{ MODKEY|ShiftMask,     43,           rotate,      { .i = -1 } },
	{ MODKEY|ShiftMask,     44,           movetab,     { .i = -1 } },
	{ MODKEY|ShiftMask,     45,           movetab,     { .i = +1 } },
	{ MODKEY,               23,           rotate,      { .i = 0 } },
	{ MODKEY,               49,           spawn,       SETPROP("_TABBED_SELECT_TAB") },
	{ MODKEY,               10,           move,        { .i = 0 } },
	{ MODKEY,               11,           move,        { .i = 1 } },
	{ MODKEY,               12,           move,        { .i = 2 } },
	{ MODKEY,               13,           move,        { .i = 3 } },
	{ MODKEY,               14,           move,        { .i = 4 } },
	{ MODKEY,               15,           move,        { .i = 5 } },
	{ MODKEY,               16,           move,        { .i = 6 } },
	{ MODKEY,               17,           move,        { .i = 7 } },
	{ MODKEY,               18,           move,        { .i = 8 } },
	{ MODKEY,               19,           move,        { .i = 9 } },
	{ MODKEY,               24,           killclient,  { 0 } },
	{ MODKEY,               30,           focusurgent, { .v = NULL } },
	{ MODKEY|ShiftMask,     30,           toggle,      { .v = (void*) &urgentswitch } },
	{ 0,                    95,           fullscreen,  { 0 } },
	#if HIDETABS_PATCH
	{ MODKEY,               50,           showbar,     { .i = 1 } },
	{ ShiftMask,            37,           showbar,     { .i = 1 } },
	#endif // HIDETABS_PATCH
};
#else
static const Key keys[] = {
	/* modifier             key           function     argument */
	{ MODKEY|ShiftMask,     XK_Return,    focusonce,   { 0 } },
	{ MODKEY|ShiftMask,     XK_Return,    spawn,       { 0 } },

	{ MODKEY|ShiftMask,     XK_l,         rotate,      { .i = +1 } },
	{ MODKEY|ShiftMask,     XK_h,         rotate,      { .i = -1 } },
	{ MODKEY|ShiftMask,     XK_j,         movetab,     { .i = -1 } },
	{ MODKEY|ShiftMask,     XK_k,         movetab,     { .i = +1 } },
	{ MODKEY,               XK_Tab,       rotate,      { .i = 0 } },

	{ MODKEY|ShiftMask,     XK_comma,     spawn,       SETPROP("_TABBED_SELECT_TAB") },
	{ MODKEY,               XK_1,         move,        { .i = 0 } },
	{ MODKEY,               XK_2,         move,        { .i = 1 } },
	{ MODKEY,               XK_3,         move,        { .i = 2 } },
	{ MODKEY,               XK_4,         move,        { .i = 3 } },
	{ MODKEY,               XK_5,         move,        { .i = 4 } },
	{ MODKEY,               XK_6,         move,        { .i = 5 } },
	{ MODKEY,               XK_7,         move,        { .i = 6 } },
	{ MODKEY,               XK_8,         move,        { .i = 7 } },
	{ MODKEY,               XK_9,         move,        { .i = 8 } },
	{ MODKEY,               XK_0,         move,        { .i = 9 } },
	{ MODKEY,               XK_q,         killclient,  { 0 } },

	{ MODKEY,               XK_u,         focusurgent, { 0 } },
	{ MODKEY|ShiftMask,     XK_u,         toggle,      { .v = (void*) &urgentswitch } },

	{ 0,                    XK_F11,       fullscreen,  { 0 } },
	#if REPARENT_PATCH
	{ MODKEY|ShiftMask,    XK_m,      spawn,       OPENTERMSOFT("_TABBED_SELECT_TERMAPP") },
	{ MODKEY|ShiftMask,    XK_u,      spawn,       OPENTERM("_TABBED_TERM") },
	{ MODKEY|ShiftMask,    XK_a,	  spawn,       ATTACHWIN("_TABBED_ATTACH_WIN") },
	{ MODKEY|ShiftMask,    XK_f,	  spawn,       ATTACHSELECTWIN("_TABBED_ATTACH_WIN") },
	{ MODKEY|ShiftMask,    XK_g,      spawn,       ATTACHALL("_TABBED_ATTACH_ALL") },
	{ MODKEY|ShiftMask,    XK_c,      spawn,       STACKTABBED("_TABBED_STACK_TABBED") },
	{ MODKEY|ShiftMask,    XK_d,	  spawn,       DETACHWIN("_TABBED_DETACH_WIN") },
	{ MODKEY|ShiftMask,    XK_z,      spawn,       DETACHALL("_TABBED_DETACH_ALL") },
	{ MODKEY|ShiftMask,    XK_w,      spawn,       HIDEWINDOW("_TABBED_HIDE_WINDOW") },
	{ MODKEY|ShiftMask,    XK_r,      spawn,       SHOWHIDDEN("_TABBED_SHOW_HIDDEN") },
	{ MODKEY|ShiftMask,    XK_q,      spawn,       SHOWHIDDENALL("_TABBED_SHOW_HIDDEN_ALL") },
	#endif // REPARENT_PATCH
	#if HIDETABS_PATCH
	{ MODKEY,               XK_Shift_L,   showbar,     { .i = 1 } },
	{ ShiftMask,            XK_Control_L, showbar,     { .i = 1 } },
	#endif // HIDETABS_PATCH
};
#endif // KEYCODE_PATCH

#if KEYRELEASE_PATCH
static const Key keyreleases[] = {
	/* modifier             key             function     argument */
	#if HIDETABS_PATCH
	{ MODKEY|ShiftMask,     XK_Shift_L,     showbar,     { .i = 0 } },
	{ MODKEY|ShiftMask,     XK_Control_L,   showbar,     { .i = 0 } },
	#else
	{ 0,                    XK_Shift_L,     NULL,        { 0 } },
	#endif // HIDETABS_PATCH

};
#endif // KEYRELEASE_PATCH
