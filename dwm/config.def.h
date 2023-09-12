/* See LICENSE file for copyright and license details. */

/* alt-tab configuration */
static const unsigned int tabModKey 		= 0x40;	/* if this key is hold the alt-tab functionality stays acitve. This key must be the same as key that is used to active functin altTabStart `*/
static const unsigned int tabCycleKey 		= 0x17;	/* if this key is hit the alt-tab program moves one position forward in clients stack. This key must be the same as key that is used to active functin altTabStart */
static const unsigned int tabPosY 			= 1;	/* tab position on Y axis, 0 = bottom, 1 = center, 2 = top */
static const unsigned int tabPosX 			= 1;	/* tab position on X axis, 0 = left, 1 = center, 2 = right */
static const unsigned int maxWTab 			= 600;	/* tab menu width */
static const unsigned int maxHTab 			= 200;	/* tab menu height */

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int gappx     = 5;        /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int user_bh           = 2;        /* 2 is the default spacing around the bar's font */
static const char buttonbar[]       = "󰕰 Start";
#define ICONSIZE bh   /* icon size */
#define ICONSPACING 5 /* space between icon and title */
static const int focusonwheel       = 0;
static const char *fonts[]          = { "Hack Nerd Font:size=11" };
static const char dmenufont[]       = "Hack Nerd Font:size=11";
static const char col_color1[]       = "#173f4f";
static const char col_color2[]       = "#173f4f";
static const char col_color3[]       = "#ffffff";
static const char col_color4[]       = "#eeeeee";
static const char col_color5[]       = "#124f5f";
static const char col_color6[]	     = "#cccccc";
static const unsigned int baralpha = 0xd0;
static const unsigned int borderalpha = OPAQUE;
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_color3, col_color1, col_color2 },
	[SchemeSel]  = { col_color4, col_color5,  col_color5  },
	[SchemeHid]  = { col_color6,  col_color1, col_color5  },
};
static const unsigned int alphas[][3]      = {
	/*               fg      bg        border     */
	[SchemeNorm] = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
static const char *alttags[] = { "<01>", "<02>", "<03>", "<04>", "<05>" };

static const unsigned int ulinepad	= 5;	/* horizontal padding between the underline and tag */
static const unsigned int ulinestroke	= 1;	/* thickness / height of the underline */
static const unsigned int ulinevoffset	= 0;	/* how far above the bottom of the bar the line should appear */
static const int ulineall 		= 0;	/* 1 to show underline on all tags, 0 for just the active ones */



static const char ptagf[] = "[%s %s]";	/* format of a tag label */
static const char etagf[] = "[%s]";	/* format of an empty tag */
static const int lcaselbl = 0;		/* 1 means make tag label lowercase */
static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      	     instance    title    tags mask     isfloating   CenterThisWindow?     monitor */
	{ "st",              NULL,       NULL,    0,            0,     	     0,		           -1 },
	{ "Gimp",            NULL,       NULL,    0,            0,           0,                    -1 },
	{ "Firefox",         NULL,       NULL,    1 << 8,       0,           0,                    -1 },
};

/* window following */
#define WFACTIVE '>'
#define WFINACTIVE 'v'
#define WFDEFAULT WFINACTIVE

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
static const int decorhints  = 1;    /* 1 means respect decoration hints */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "HHH",      grid },
	{ "-#-",      horizgrid },
	{ "###",      gaplessgrid },
};

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ Mod4Mask,                     XK_s,      spawn,          SHCMD("spmenu_run -d -a '-g 4 -l 10'") },
	{ Mod4Mask|ShiftMask,		XK_s,      spawn,          SHCMD("spmenu_run -a '-g 4 -l 10'") },
	{ Mod4Mask|Mod1Mask,		XK_s,	   spawn,	   SHCMD("dmenu_run -fn 'Hack Nerd Font:size=11' -nb '#173f4f' -nf '#ffffff' -sb '#124f5f' -sf '#eeeeee' -g 6 -l 6") },
	{ Mod4Mask,                     XK_f,      spawn,          SHCMD("spmenu_run -fm -a '-g 4 -l 10'") },
	/* needs spmenu */
	{ Mod4Mask,			XK_v,	   spawn,	   SHCMD("clipmenu-spmenu") },
	{ Mod4Mask|ShiftMask,		XK_Print,  spawn,	   SHCMD("screenshot-spmenu -f") },
	{ Mod4Mask|Mod1Mask,		XK_Print,  spawn,	   SHCMD("screenshot-spmenu -s") },  
	{ Mod4Mask,			XK_p,	   spawn,	   SHCMD("pirokit") },
	{ Mod4Mask|ShiftMask,		XK_w,	   spawn,	   SHCMD("wallpaper-spmenu") },
	{ Mod4Mask,			XK_n,	   spawn,	   SHCMD("dunstctl history-pop") },
	{ Mod4Mask,			XK_x,	   spawn,	   SHCMD("dunstctl close-all") },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          SHCMD("st -e") },
	{ MODKEY|ShiftMask,             XK_t,      spawn,          SHCMD("tabbed -r 2 st -w ''") },
    { MODKEY|ShiftMask,             XK_i,      spawn,          SHCMD("firefox") },
	{ Mod4Mask,                     XK_e,      spawn,          SHCMD("st -T sfm sfm") },
    { Mod4Mask,                     XK_Print,  spawn,          SHCMD("maim ~/Pictures/Screenshot_$(date +%s).png") },
	{ MODKEY|ShiftMask,             XK_b,      togglebar,      {0} },
	{ MODKEY|ShiftMask,             XK_n,      togglefollow,   {0} },
	{ MODKEY,           	        XK_j,      focusstackhid,  {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstackhid,  {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_q,      view,           {0} },
	{ MODKEY|ShiftMask,             XK_x,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_g,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY|ShiftMask,             XK_d,      setlayout,      {.v = &layouts[4]} },
	{ MODKEY|ShiftMask,             XK_f,      setlayout,      {.v = &layouts[5]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY|ShiftMask,             XK_f,      togglefullscr,  {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ ControlMask|ShiftMask,        XK_h,      togglewin,      {0} },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_h,      layoutscroll,   {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_l,      layoutscroll,   {.i = +1 } },
	{ MODKEY,                       XK_minus,  setgaps,        {.i = -1 } },
	{ MODKEY,                       XK_equal,  setgaps,        {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_equal,  setgaps,        {.i = 0  } },
	{ MODKEY,             		    XK_Tab,    altTabStart,	   {0} },
	{ MODKEY,                       XK_n,      shiftview,      { .i = +1 } },
	{ MODKEY,                       XK_b,      shiftview,      { .i = -1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkButton,	        0,	        Button1,	spawn,		SHCMD("spmenu_run -d -a '-g 4 -l 10'") },
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkFollowSymbol,      0,              Button1,        togglefollow,   {0} },
	{ ClkWinTitle,          0,              Button1,        togglewin,      {0} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          SHCMD("st -e") },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

