int
width_winbutton(Bar *bar, BarArg *a)
{
	return TEXTW(buttonbar6);
}

int
draw_winbutton(Bar *bar, BarArg *a)
{
	return drw_text(drw, a->x, a->y, a->w, a->h, lrpad / 2, buttonbar6, 0, False);
}

int
click_winbutton(Bar *bar, Arg *arg, BarArg *a)
{
	return ClkButton6;
}
int
width_winbutton2(Bar *bar, BarArg *a)
{
	return TEXTW(buttonbar7);
}

int
draw_winbutton2(Bar *bar, BarArg *a)
{
	return drw_text(drw, a->x, a->y, a->w, a->h, lrpad / 2, buttonbar7, 0, False);
}

int
click_winbutton2(Bar *bar, Arg *arg, BarArg *a)
{
	return ClkButton7;
}
int
width_winbutton3(Bar *bar, BarArg *a)
{
	return TEXTW(buttonbar8);
}

int
draw_winbutton3(Bar *bar, BarArg *a)
{
	return drw_text(drw, a->x, a->y, a->w, a->h, lrpad / 2, buttonbar8, 0, False);
}

int
click_winbutton3(Bar *bar, Arg *arg, BarArg *a)
{
	return ClkButton8;
}
