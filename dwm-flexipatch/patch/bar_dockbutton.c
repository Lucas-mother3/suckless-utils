int
width_dcbutton(Bar *bar, BarArg *a)
{
	return TEXTW(buttonbar2);
}

int
draw_dcbutton(Bar *bar, BarArg *a)
{
	return drw_text(drw, a->x, a->y, a->w, a->h, lrpad / 2, buttonbar2, 0, False);
}

int
click_dcbutton(Bar *bar, Arg *arg, BarArg *a)
{
	return ClkButton2;
}
int width_dcbutton2(Bar *bar, BarArg *a)
{
	return TEXTW(buttonbar3);
}

int
draw_dcbutton2(Bar *bar, BarArg *a)
{
	return drw_text(drw, a->x, a->y, a->w, a->h, lrpad / 2, buttonbar3, 0, False);
}

int
click_dcbutton2(Bar *bar, Arg *arg, BarArg *a)
{
	return ClkButton3;
}
int
width_dcbutton3(Bar *bar, BarArg *a)
{
	return TEXTW(buttonbar4);
}

int
draw_dcbutton3(Bar *bar, BarArg *a)
{
	return drw_text(drw, a->x, a->y, a->w, a->h, lrpad / 2, buttonbar4, 0, False);
}

int
click_dcbutton3(Bar *bar, Arg *arg, BarArg *a)
{
	return ClkButton4;
}
