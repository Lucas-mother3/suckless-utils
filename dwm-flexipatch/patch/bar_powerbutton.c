int
width_powbutton(Bar *bar, BarArg *a)
{
	return TEXTW(buttonbar5);
}

int
draw_powbutton(Bar *bar, BarArg *a)
{
	return drw_text(drw, a->x, a->y, a->w, a->h, lrpad / 20, buttonbar5, 0, False);
}

int
click_powbutton(Bar *bar, Arg *arg, BarArg *a)
{
	return ClkButton5;
}
