int
width_stbutton(Bar *bar, BarArg *a)
{
	return TEXTW(buttonbar);
}

int
draw_stbutton(Bar *bar, BarArg *a)
{
	return drw_text(drw, a->x, a->y, a->w, a->h, lrpad / 2, buttonbar, 0, False);
}

int
click_stbutton(Bar *bar, Arg *arg, BarArg *a)
{
	return ClkButton;
}
/* modification by Lucas-mother3 to add more buttons */
int
width_stbutton2(Bar *bar, BarArg *a)
{
	return TEXTW(buttonbar2);
}

int
draw_stbutton2(Bar *bar, BarArg *a)
{
	return drw_text(drw, a->x, a->y, a->w, a->h, lrpad / 2, buttonbar2, 0, False);
}

int
click_stbutton2(Bar *bar, Arg *arg, BarArg *a)
{
	return ClkButton2;
}
int
width_stbutton3(Bar *bar, BarArg *a)
{
	return TEXTW(buttonbar3);
}

int
draw_stbutton3(Bar *bar, BarArg *a)
{
	return drw_text(drw, a->x, a->y, a->w, a->h, lrpad / 2, buttonbar3, 0, False);
}

int
click_stbutton3(Bar *bar, Arg *arg, BarArg *a)
{
	return ClkButton3;
}
int
width_stbutton4(Bar *bar, BarArg *a)
{
	return TEXTW(buttonbar4);
}

int
draw_stbutton4(Bar *bar, BarArg *a)
{
	return drw_text(drw, a->x, a->y, a->w, a->h, lrpad / 2, buttonbar4, 0, False);
}

int
click_stbutton4(Bar *bar, Arg *arg, BarArg *a)
{
	return ClkButton4;
}
int
width_stbutton5(Bar *bar, BarArg *a)
{
	return TEXTW(buttonbar5);
}

int
draw_stbutton5(Bar *bar, BarArg *a)
{
	return drw_text(drw, a->x, a->y, a->w, a->h, lrpad / 2, buttonbar5, 0, False);
}

int
click_stbutton5(Bar *bar, Arg *arg, BarArg *a)
{
	return ClkButton5;
}
int
width_stbutton6(Bar *bar, BarArg *a)
{
	return TEXTW(buttonbar6);
}

int
draw_stbutton6(Bar *bar, BarArg *a)
{
	return drw_text(drw, a->x, a->y, a->w, a->h, lrpad / 2, buttonbar6, 0, False);
}

int
click_stbutton6(Bar *bar, Arg *arg, BarArg *a)
{
	return ClkButton6;
}
int
width_stbutton7(Bar *bar, BarArg *a)
{
	return TEXTW(buttonbar7);
}

int
draw_stbutton7(Bar *bar, BarArg *a)
{
	return drw_text(drw, a->x, a->y, a->w, a->h, lrpad / 2, buttonbar7, 0, False);
}

int
click_stbutton7(Bar *bar, Arg *arg, BarArg *a)
{
	return ClkButton7;
}
int
width_stbutton8(Bar *bar, BarArg *a)
{
	return TEXTW(buttonbar8);
}

int
draw_stbutton8(Bar *bar, BarArg *a)
{
	return drw_text(drw, a->x, a->y, a->w, a->h, lrpad / 2, buttonbar8, 0, False);
}

int
click_stbutton8(Bar *bar, Arg *arg, BarArg *a)
{
	return ClkButton8;
}
