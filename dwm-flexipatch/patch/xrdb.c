void
loadxrdb()
{
	Display *display;
	char * resm;
	XrmDatabase xrdb;
	char *type;
	XrmValue value;

	display = XOpenDisplay(NULL);

	if (display != NULL) {
		resm = XResourceManagerString(display);

		if (resm != NULL) {
			xrdb = XrmGetStringDatabase(resm);

			if (xrdb != NULL) {
				XRDB_LOAD_COLOR("dwm.color15", normfgcolor);
				XRDB_LOAD_COLOR("dwm.color0", normbgcolor);
				XRDB_LOAD_COLOR("dwm.color10", normbordercolor);
				XRDB_LOAD_COLOR("dwm.color10", normfloatcolor);
				XRDB_LOAD_COLOR("dwm.color0", selfgcolor);
				XRDB_LOAD_COLOR("dwm.color14", selbgcolor);
				XRDB_LOAD_COLOR("dwm.color12", selbordercolor);
				XRDB_LOAD_COLOR("dwm.color12", selfloatcolor);
				XRDB_LOAD_COLOR("dwm.color15", titlenormfgcolor);
				XRDB_LOAD_COLOR("dwm.color0", titlenormbgcolor);
				XRDB_LOAD_COLOR("dwm.color10", titlenormbordercolor);
				XRDB_LOAD_COLOR("dwm.color10", titlenormfloatcolor);
				XRDB_LOAD_COLOR("dwm.color0", titleselfgcolor);
				XRDB_LOAD_COLOR("dwm.color14", titleselbgcolor);
				XRDB_LOAD_COLOR("dwm.color12", titleselbordercolor);
				XRDB_LOAD_COLOR("dwm.color12", titleselfloatcolor);
				XRDB_LOAD_COLOR("dwm.color15", tagsnormfgcolor);
				XRDB_LOAD_COLOR("dwm.color0", tagsnormbgcolor);
				XRDB_LOAD_COLOR("dwm.color10", tagsnormbordercolor);
				XRDB_LOAD_COLOR("dwm.color10", tagsnormfloatcolor);
				XRDB_LOAD_COLOR("dwm.color0", tagsselfgcolor);
				XRDB_LOAD_COLOR("dwm.color14", tagsselbgcolor);
				XRDB_LOAD_COLOR("dwm.color12", tagsselbordercolor);
				XRDB_LOAD_COLOR("dwm.color12", tagsselfloatcolor);
				XRDB_LOAD_COLOR("dwm.color15", hidnormfgcolor);
				XRDB_LOAD_COLOR("dwm.color0", hidnormbgcolor);
				XRDB_LOAD_COLOR("dwm.color0", hidselfgcolor);
				XRDB_LOAD_COLOR("dwm.color14", hidselbgcolor);
				XRDB_LOAD_COLOR("dwm.color15", urgfgcolor);
				XRDB_LOAD_COLOR("dwm.color3", urgbgcolor);
				XRDB_LOAD_COLOR("dwm.color11", urgbordercolor);
				XRDB_LOAD_COLOR("dwm.color11", urgfloatcolor);
				#if RENAMED_SCRATCHPADS_PATCH
				XRDB_LOAD_COLOR("dwm.color0", scratchselfgcolor);
				XRDB_LOAD_COLOR("dwm.color1", scratchselbgcolor);
				XRDB_LOAD_COLOR("dwm.color1", scratchselbordercolor);
				XRDB_LOAD_COLOR("dwm.color1", scratchselfloatcolor);
				XRDB_LOAD_COLOR("dwm.color0", scratchnormfgcolor);
				XRDB_LOAD_COLOR("dwm.color4", scratchnormbgcolor);
				XRDB_LOAD_COLOR("dwm.color4", scratchnormbordercolor);
				XRDB_LOAD_COLOR("dwm.color4", scratchnormfloatcolor);
				#endif // RENAMED_SCRATCHPADS_PATCH
				#if BAR_FLEXWINTITLE_PATCH
				XRDB_LOAD_COLOR("dwm.color0", normTTBbgcolor);
				XRDB_LOAD_COLOR("dwm.color0", normLTRbgcolor);
				XRDB_LOAD_COLOR("dwm.color0", normMONObgcolor);
				XRDB_LOAD_COLOR("dwm.color0", normGRIDbgcolor);
				XRDB_LOAD_COLOR("dwm.color0", normGRD1bgcolor);
				XRDB_LOAD_COLOR("dwm.color0", normGRD2bgcolor);
				XRDB_LOAD_COLOR("dwm.color0", normGRDMbgcolor);
				XRDB_LOAD_COLOR("dwm.color0", normHGRDbgcolor);
				XRDB_LOAD_COLOR("dwm.color0", normDWDLbgcolor);
				XRDB_LOAD_COLOR("dwm.color0", normSPRLbgcolor);
				XRDB_LOAD_COLOR("dwm.color0", normfloatbgcolor);
				XRDB_LOAD_COLOR("dwm.color2", actTTBbgcolor);
				XRDB_LOAD_COLOR("dwm.color2", actLTRbgcolor);
				XRDB_LOAD_COLOR("dwm.color2", actMONObgcolor);
				XRDB_LOAD_COLOR("dwm.color2", actGRIDbgcolor);
				XRDB_LOAD_COLOR("dwm.color2", actGRD1bgcolor);
				XRDB_LOAD_COLOR("dwm.color2", actGRD2bgcolor);
				XRDB_LOAD_COLOR("dwm.color2", actGRDMbgcolor);
				XRDB_LOAD_COLOR("dwm.color2", actHGRDbgcolor);
				XRDB_LOAD_COLOR("dwm.color2", actDWDLbgcolor);
				XRDB_LOAD_COLOR("dwm.color2", actSPRLbgcolor);
				XRDB_LOAD_COLOR("dwm.color2", actfloatbgcolor);
				XRDB_LOAD_COLOR("dwm.color6", selTTBbgcolor);
				XRDB_LOAD_COLOR("dwm.color6", selLTRbgcolor);
				XRDB_LOAD_COLOR("dwm.color6", selMONObgcolor);
				XRDB_LOAD_COLOR("dwm.color6", selGRIDbgcolor);
				XRDB_LOAD_COLOR("dwm.color6", selGRD1bgcolor);
				XRDB_LOAD_COLOR("dwm.color6", selGRD2bgcolor);
				XRDB_LOAD_COLOR("dwm.color6", selGRDMbgcolor);
				XRDB_LOAD_COLOR("dwm.color6", selHGRDbgcolor);
				XRDB_LOAD_COLOR("dwm.color6", selDWDLbgcolor);
				XRDB_LOAD_COLOR("dwm.color6", selSPRLbgcolor);
				XRDB_LOAD_COLOR("dwm.color6", selfloatbgcolor);
				#endif // BAR_FLEXWINTITLE_PATCH
				#if BAR_STATUS2D_XRDB_TERMCOLORS_PATCH && BAR_STATUS2D_PATCH
				XRDB_LOAD_COLOR("color0", termcol0);
				XRDB_LOAD_COLOR("color1", termcol1);
				XRDB_LOAD_COLOR("color2", termcol2);
				XRDB_LOAD_COLOR("color3", termcol3);
				XRDB_LOAD_COLOR("color4", termcol4);
				XRDB_LOAD_COLOR("color5", termcol5);
				XRDB_LOAD_COLOR("color6", termcol6);
				XRDB_LOAD_COLOR("color7", termcol7);
				XRDB_LOAD_COLOR("color8", termcol8);
				XRDB_LOAD_COLOR("color9", termcol9);
				XRDB_LOAD_COLOR("color10", termcol10);
				XRDB_LOAD_COLOR("color11", termcol11);
				XRDB_LOAD_COLOR("color12", termcol12);
				XRDB_LOAD_COLOR("color13", termcol13);
				XRDB_LOAD_COLOR("color14", termcol14);
				XRDB_LOAD_COLOR("color15", termcol15);
				#endif // BAR_STATUS2D_XRDB_TERMCOLORS_PATCH

				XrmDestroyDatabase(xrdb);
			}
		}
	}

	XCloseDisplay(display);
}

void
xrdb(const Arg *arg)
{
	loadxrdb();
	int i;
	for (i = 0; i < LENGTH(colors); i++)
		scheme[i] = drw_scm_create(drw, colors[i],
		#if BAR_ALPHA_PATCH
		alphas[i],
		#endif // BAR_ALPHA_PATCH
		ColCount
		);
	arrange(NULL);
	focus(NULL);
}
