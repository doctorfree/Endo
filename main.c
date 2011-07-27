/*************************************************************************
 *                                                                       *
 *  Copyright (c) 1992-2011 Ronald Joe Record                           *
 *                                                                       *
 *************************************************************************/
 /*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  The X Consortium, and any party obtaining a copy of these files from
 *  the X Consortium, directly or indirectly, is granted, free of charge,
 *  a full and unrestricted irrevocable, world-wide, paid up, royalty-free,
 *  nonexclusive right and license to deal in this software and
 *  documentation files (the "Software"), including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons who receive
 *  copies from any such party to do so.  This license includes without
 *  limitation a license to do the foregoing actions under any patents of
 *  the party supplying this software to the X Consortium.
 */

/* Endo - iterate an endomorphism of the plane */

/* Written by Ron Record (rr@sco) 17 Jan 1992 */

/* INTRO
 * -----
 * The basic idea is to iterate an endomorphism of the plane (R x R).
 * Each point (x,y) is mapped to (f(x,y),g(x,y)). The new point is
 * then mapped to (f(f(x,y),g(x,y)),g(f(x,y),g(x,y))) and so on. Each
 * point is then colored according to how many iterates it took to get
 * within some "delta" of a previous iterate (e.g. a periodic orbit is
 * detected). In addition, another window can be "popped up" and a display
 * of each point's trajectory viewed. A third window displaying the iterates
 * of the "critical curve" can also be popped up. The critical curve is 
 * composed of those points where the determinant of the jacobian is zero.
 * A fourth window displaying Lyapunov exponent values for a region of
 * parameter space may also be viewed.
 * 
 * ACKNOWLEDGEMENTS
 * ----------------
 * The Winter 1992 seminar at UCSC led by Dr. Ralph Abraham was the
 * inspiration for the study of iterated endomorphisms of the plane.
 * Techniques for calculating the critical arcs and absorbing regions
 * were drawn from Laura Gardini of the Istituto di Scienze economiche,
 * Universita' di Urbino. Studies of various maps of the plane were used
 * including papers by Jon von Neumann, Graciela Chichilnisky, John 
 * Guckenheimer, G. Oster, A. Ipaktchi, Wulf Gaertner, Jochen Jungeilges,
 * Ralph Abraham, B. P. Wood, A. J. Lichtenberg, Z. You, E.J. Kostelich,
 * J. A. Yorke, R. Brown, L. Chua, K. Judd, A. I. Mees, K. Aihara, and
 * M. Toyoda. For a complete bibliography, see the README.
 */

#include "patchlevel.h"
#include "x.h"

static char *version = ENDO_VERSION;

int demo = 0, cdown = 1, pdown = 1, adown = 1, ldown = 1, tdown = 1, idown = 1;
int nonleg = 0, updt = 0, usedefault = 0;
static const char wname[256];
static XSizeHints hint;
extern int MapSelect(), Tit_Event();
extern void Show_Maps();

main(ac, av)
	int ac;
	char **av;
{
	static int ascent, descent, dir, i;
	static XCharStruct overall;
	static XWindowChanges values;
	extern int axes, numattrs;
	extern void init_data(), init_color();
	extern void Clear(), parseargs(), Save_Parms();
	extern void Draw_Axes(), Draw_Diagonal();
	XVisualInfo *visual_list, visual_template;

	parseargs(ac, av);
	dpy = XOpenDisplay("");
	screen = DefaultScreen(dpy);
	background = BlackPixel(dpy, screen);
	if (mapindex == -1) {
		Save_Parms(1);
		Change_Parms(0);
	}
	setupmem();
	init_data();
	CreateXorGC(which); 
	if (displayplanes > 1)
		foreground = STARTCOLOR;
	else
		foreground = WhitePixel(dpy,XDefaultScreen(dpy));
	hint.flags = PPosition|PSize;
	/*
	* Create the window/pixmap used to display the title screen trajectories
	*/
	hint.x = 0;
	hint.y = 0;
	hint.width = XDisplayWidth(dpy, screen);
	hint.height = XDisplayHeight(dpy, screen);
	pixtra = XCreatePixmap(dpy, DefaultRootWindow(dpy), hint.width, 
				hint.height, DefaultDepth(dpy, screen));
	trajec = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy),
				hint.x, hint.y, hint.width, hint.height,
				5, foreground, background);
	XSetNormalHints(dpy, trajec, &hint);
	XSelectInput(dpy,trajec,KeyPressMask|ButtonPressMask|ExposureMask);
	/*
	* Create the window used to display the Lyapunov exponents
	*/
	hint.width = width;
	hint.height = height;
	pixmap = XCreatePixmap(dpy, DefaultRootWindow(dpy), width, height, 
					DefaultDepth(dpy, screen));
	lyajec = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), hint.x, hint.y, 
					width, height, 5, foreground, background);
	/* Title */
	sprintf((char *) wname, LYAP_NAME);
  	XSetStandardProperties(dpy, lyajec, wname, "Lyapunov", None, av, ac, &hint);
	XSetNormalHints(dpy, lyajec, &hint);
	visual_template.class = PseudoColor;
	visual_list = XGetVisualInfo(dpy, VisualClassMask, &visual_template, &i);
	if (! visual_list)
	  usedefault = 1;
	if (usedefault)
	  cmap = DefaultColormap(dpy, screen);
	else
	  cmap = XCreateColormap(dpy, trajec, DefaultVisual(dpy, screen), AllocAll);
	if (mapindex == -1) {
		XMapRaised(dpy, lyajec);
		if (!usedefault)
		    init_color(dpy, lyajec, cmap, Colors, STARTCOLOR, MINCOLINDEX,
					numcolors, numwheels, "endo", "Endo", numattrs);
		ldown=0;
		Clear(lyajec);
		which = lyajec;
		Title_Page();
	}
	if (pixmap)	/* reallocate lyapunov pixmap if necessary */
		XFreePixmap(dpy, pixmap);
	pixmap = XCreatePixmap(dpy, DefaultRootWindow(dpy), 
			width, height, DefaultDepth(dpy, screen));
	values.x = Max(XDisplayWidth(dpy, screen) - width, 0);
	values.y = Max(XDisplayHeight(dpy, screen) - height, 0);
	values.width = width;
	values.height = height;
	XConfigureWindow(dpy,lyajec,CWX|CWY|CWWidth|CWHeight,&values);
	/* Title */
	XStoreName(dpy, lyajec, LYAP_NAME);
	if (pixtra)	/* reallocate trajectory pixmap if necessary */
		XFreePixmap(dpy, pixtra);
	pixtra = XCreatePixmap(dpy, DefaultRootWindow(dpy), 
			trawidth, traheight, DefaultDepth(dpy, screen));
	values.x = Max((XDisplayWidth(dpy, screen)/2) - (width/2), 0);
	values.y = Max(INFO_HEIGHT, (XDisplayHeight(dpy, screen) / 2) -
			(height / 2)) + 30;
	values.width = trawidth;
	values.height = traheight;
	XConfigureWindow(dpy,trajec,CWX|CWY|CWWidth|CWHeight,&values);
	/* Title */
	if (lyap)
		sprintf((char *) wname, BIF_NAME);
	else if (histogram)
		sprintf((char *) wname, HIST_NAME);
	else
		sprintf((char *) wname, TRAJ_NAME);
  	XSetStandardProperties(dpy, trajec, wname, "Orbits", None, av, ac, &hint);
	/*
 	* Create the window used to display the rate of attraction
 	*/
	hint.x = 0;
	hint.y = Max(XDisplayHeight(dpy,screen) - height, 0);
	canvas = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), hint.x, hint.y, 
					width, height, 5, foreground, background);
	pixmap = XCreatePixmap(dpy, DefaultRootWindow(dpy), 
			width, height, DefaultDepth(dpy, screen));
	/* Title */
	sprintf((char *) wname, ATTR_NAME);
  	XSetStandardProperties(dpy, canvas, wname, "Endo", None, av, ac, &hint);
	XSetNormalHints(dpy, canvas, &hint);
	/*
	* Create the window used to display the critical curves
	*/
	hint.x = Max(XDisplayWidth(dpy, screen) - criwidth, 0);
	hint.y = 0;
	crijec = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), hint.x, hint.y, 
					criwidth, criheight, 5, foreground, background);
	pixcri = XCreatePixmap(dpy, DefaultRootWindow(dpy), 
			criwidth, criheight, DefaultDepth(dpy, screen));
	/* Title */
	sprintf((char *) wname, CRIT_NAME);
  	XSetStandardProperties(dpy, crijec, wname, "Curves", None, av, ac, &hint);
	XSetNormalHints(dpy, crijec, &hint);
	/*
	* Create the window used to adjust the color map indexing
	*/
	hint.x = XDisplayWidth(dpy, screen) - ADJWIDTH - 20;
	hint.y = XDisplayHeight(dpy, screen) - ADJHEIGHT - 20;
	colwin = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), hint.x, hint.y, 
					ADJWIDTH, ADJHEIGHT, 5, foreground, background);
	/* Title */
	sprintf((char *) wname, ADJ_NAME);
  	XSetStandardProperties(dpy, colwin, wname, "Index Map", None,av,ac,&hint);
	XSetNormalHints(dpy, colwin, &hint);
	/*
	* Create the window used to display the color bar histogram index
	*/
	hint.x = Max(XDisplayWidth(dpy, screen) - HISBARWID - 20, 0);
	hint.y = 0;
	hisbar = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), hint.x, hint.y, 
					HISBARWID, HISBARHGT, 5, foreground, background);
	/* Title */
	sprintf((char *) wname, BAR_NAME);
  	XSetStandardProperties(dpy, hisbar, wname, "Color Index", None,av,ac,&hint);
	XSetNormalHints(dpy, hisbar, &hint);
	/*
	* Create the window used to display pre-iterates of the critical curves
	*/
	hint.x = 0;
	hint.y = 0;
	prejec = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), hint.x, hint.y, 
					width, height, 5, foreground, background);
	pixpre = XCreatePixmap(dpy, DefaultRootWindow(dpy), 
			width, height, DefaultDepth(dpy, screen));
	/* Title */
	sprintf((char *) wname, PRE_NAME);
  	XSetStandardProperties(dpy, prejec, wname, "Pre-Curves", None,av,ac,&hint);
	XSetNormalHints(dpy, prejec, &hint);
	/*
	* Create the window used to display the parameter and map information
	*/
	if (displayplanes > 1)
		XQueryTextExtents(dpy,(XID)XGContextFromGC(Data_GC[CG]),"X",
			1,&dir,&ascent,&descent,&overall);
	else
		XQueryTextExtents(dpy,(XID)XGContextFromGC(Data_GC[1]),"X",
			1,&dir,&ascent,&descent,&overall);
	hint.x = 0;
	hint.y = 0;
	hint.width = INFO_WIDTH * overall.width;
	hint.height = INFO_HEIGHT * (ascent + descent);
	info = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), hint.x, hint.y, 
					hint.width, hint.height, 5, foreground, background);
	/* Title */
	sprintf((char *) wname, INFO_NAME);
  	XSetStandardProperties(dpy, info, wname, "Info", None, av, ac, &hint);
	XSetNormalHints(dpy, info, &hint);
	/*
	* Create the window used to display the help info
	*/
	hint.x = hint.width + 20;
	hint.width = HELP_WIDTH * overall.width;
	hint.height = HELP_HEIGHT * (ascent + descent);
	help = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), hint.x, hint.y, 
					hint.width, hint.height, 5, foreground, background);
	/* Title */
	sprintf((char *) wname, HELP_NAME);
  	XSetStandardProperties(dpy, help, wname, "Help", None, av, ac, &hint);
	XSetNormalHints(dpy, help, &hint);
	XSelectInput(dpy,lyajec,KeyPressMask|ButtonPressMask|ButtonMotionMask|
		ButtonReleaseMask|ExposureMask|StructureNotifyMask|FocusChangeMask);
	XSelectInput(dpy,trajec,KeyPressMask|ButtonPressMask|ButtonMotionMask|
		ButtonReleaseMask|ExposureMask|StructureNotifyMask|FocusChangeMask);
	XSelectInput(dpy,canvas,KeyPressMask|ButtonPressMask|ButtonMotionMask|
		ButtonReleaseMask|ExposureMask|StructureNotifyMask|FocusChangeMask);
	XSelectInput(dpy,crijec,KeyPressMask|ButtonPressMask|ButtonMotionMask|
		ButtonReleaseMask|ExposureMask|StructureNotifyMask|FocusChangeMask);
	XSelectInput(dpy,prejec,KeyPressMask|ButtonPressMask|ButtonMotionMask|
		ButtonReleaseMask|ExposureMask|StructureNotifyMask|FocusChangeMask);
	XSelectInput(dpy,colwin,KeyPressMask|ButtonPressMask|ButtonMotionMask|
		ButtonReleaseMask|ExposureMask|FocusChangeMask);
	XSelectInput(dpy,info,KeyPressMask|ExposureMask|FocusChangeMask);
	XSelectInput(dpy,help,KeyPressMask|ExposureMask|FocusChangeMask);
	XSelectInput(dpy,hisbar,KeyPressMask|ExposureMask|FocusChangeMask);
	Raise_Maps();
	if ((displayplanes > 1) && (!usedefault))
		init_color(dpy, info, cmap, Colors, STARTCOLOR, MINCOLINDEX,
					numcolors, numwheels, "endo", "Endo", numattrs);
	else
		XQueryColors(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), Colors, 
						numcolors);
	rubber_data.band_cursor=XCreateFontCursor(dpy, XC_hand2);
	Draw_Diagonal(trajec);
	Draw_Axes(trajec, axes);
	if (demo)
		for (;;) {
			main_event();
			if (!run) {
				Cleanup();
				exit(0);
			}
		}
		else
			for (;;)
				main_event();
}

int
coltohist(n, max, min)
int n, max, min;
{
	extern int start;

	return(((n*(max - min)/(numcolors - start - 1)) + min));
}

void
updtbar(win, h, H, m, max, min)
Window win;
int h, H, m, max, min;
{
	static char str[32];
	static int k, n, Y0;
	static double d;
	extern int start;
	extern double sea_level, sky_level;

	k = numcolors - start - 1;
	if (m)
		Y0 = H;
	else
		Y0 = h;
	if (nonleg) {
		d = sky_level - sea_level;
		sprintf(str, "%d", (int)(sea_level * (max-min)) + min);
		XDrawImageString(dpy,win,Data_GC[1], 0, Y0-1, str, strlen(str));
		if (win == trajec)
			XDrawImageString(dpy,pixtra,Data_GC[1], 0, Y0-1, str, strlen(str));
		sprintf(str, "%d", (int)(sky_level * (max-min)) + min);
		XDrawImageString(dpy,win,Data_GC[1], 0, Y0-h+10, str, strlen(str));
		if (win == trajec)
			XDrawImageString(dpy,pixtra,Data_GC[1],0,Y0-h+10,str,strlen(str));
		sprintf(str, "%d", 
					(int)(((0.75*d)+sea_level)*(max-min)) + min);
		XDrawImageString(dpy,win,Data_GC[1], 0, Y0-(3*h/4), str, strlen(str));
		if (win == trajec)
		   XDrawImageString(dpy,pixtra,Data_GC[1],0,Y0-(3*h/4),str,strlen(str));
		sprintf(str, "%d",
					(int)(((0.25*d)+sea_level)*(max-min)) + min);
		XDrawImageString(dpy,win,Data_GC[1], 0, Y0-(h/4), str, strlen(str));
		if (win == trajec)
			XDrawImageString(dpy,pixtra,Data_GC[1],0,Y0-(h/4),str,strlen(str));
		sprintf(str, "%d",
					(int)(((0.5*d)+sea_level)*(max-min)) + min);
		XDrawImageString(dpy,win,Data_GC[1], 0, Y0-(h/2), str, strlen(str));
		if (win == trajec)
			XDrawImageString(dpy,pixtra,Data_GC[1],0,Y0-(h/2),str,strlen(str));
	}
	else {
		n = coltohist(get_hist_index(sea_level) - start, max, min);
		sprintf(str, "%d",n);
		XDrawImageString(dpy,win,Data_GC[1], 0, Y0-1, str, strlen(str));
		if (win == trajec)
			XDrawImageString(dpy,pixtra,Data_GC[1], 0, Y0-1, str, strlen(str));
		n = coltohist(get_hist_index(sky_level)
				- start, max, min);
		sprintf(str, "%d", n);
		XDrawImageString(dpy,win,Data_GC[1], 0, Y0-h+10, str, strlen(str));
		if (win == trajec)
			XDrawImageString(dpy,pixtra,Data_GC[1],0,Y0-h+10,str,strlen(str));
		n = coltohist(get_hist_index(0.75)-start, max, min);
		sprintf(str, "%d", n);
		XDrawImageString(dpy,win,Data_GC[1], 0, Y0-(3*h/4), str, strlen(str));
		if (win == trajec)
		   XDrawImageString(dpy,pixtra,Data_GC[1],0,Y0-(3*h/4),str,strlen(str));
		n = coltohist(get_hist_index(0.25)-start, max, min);
		sprintf(str, "%d", n);
		XDrawImageString(dpy,win,Data_GC[1], 0, Y0-(h/4), str, strlen(str));
		if (win == trajec)
			XDrawImageString(dpy,pixtra,Data_GC[1],0,Y0-(h/4),str,strlen(str));
		n = coltohist(get_hist_index(0.5)-start, max, min);
		sprintf(str, "%d", n);
		XDrawImageString(dpy,win,Data_GC[1], 0, Y0-(h/2), str, strlen(str));
		if (win == trajec)
			XDrawImageString(dpy,pixtra,Data_GC[1],0,Y0-(h/2),str,strlen(str));
	}
}

void
fillbar(win, h, H, n) /* window, bar height, window height, corner */
Window win;
int h, H, n;
{
	Window r;
	static int x, y;
	unsigned int new_w, new_h, bw, d;
	static int i, j, k, Y0;
	extern int maxhist, minhist, start;

	if (win == hisbar)
		XGetGeometry(dpy,win,&r,&x,&y,&new_w,&new_h,&bw,&d);
	if (nonleg) {
		k = numcolors - start - 1;
		for (i=0; i<h; i++) {
			j = get_hist_index((double)i/(double)h);
			if (n) /* lower left corner */
				Y0 = H - i;
			else /* upper left corner */
				Y0 = h - i;
			if (win == trajec) {
				XDrawLine(dpy, win, Data_GC[j], 0, Y0, HISBARWID - 1, Y0);
				XDrawLine(dpy, pixtra, Data_GC[j], 0, Y0, HISBARWID - 1, Y0);
			}
			else
				XDrawLine(dpy, win, Data_GC[j], 0, Y0, new_w, Y0);
		}
	}
	else {
		j = h/numfreecols + 1;
		for (i=MINCOLINDEX; i<numcolors; i++) {
			if (n) /* lower left corner */
				Y0 = H-(h*(i-MINCOLINDEX)/numfreecols)-j;
			else /* upper left corner */
				Y0 = h-(h*(i-MINCOLINDEX)/numfreecols)-j;
			if (win == trajec) {
				XFillRectangle(dpy, win, Data_GC[i], 0, Y0, HISBARWID, j);
				XFillRectangle(dpy, pixtra, Data_GC[i], 0, Y0, HISBARWID, j);
			}
			else
				XFillRectangle(dpy, win, Data_GC[i], 0, Y0, new_w, j);
		}
	}
	if (updt)
		if (portrait && histogram)
			updtbar(win, h, H, n, maxhist, minhist);
		else
			updtbar(win, h, H, n, maxperiod, minperiod);
}

void
drawmap(a1, b1, a2, b2)
double a1, b1, a2, b2;
{
	static int i, j, k, l;
	static double d;
	extern int histex, start;

	if ((histex == 0) || (histex == 1) || (histex == -2)) {
		i = (int)(a1*ADJWIDTH);
		j = (int)(b1*ADJHEIGHT);
		XDrawLine(dpy, colwin, RubberGC, 0, ADJHEIGHT, i, ADJHEIGHT - j);
		if (histex == 0) {
			k = (int)(a2*ADJWIDTH);
			l = (int)(b2*ADJHEIGHT);
			XDrawLine(dpy, colwin, RubberGC, i, ADJHEIGHT-j, k, ADJHEIGHT-l);
			XDrawLine(dpy, colwin, RubberGC, k, ADJHEIGHT-l, ADJWIDTH, 0);
		}
		else if (histex == -2) {
			XDrawLine(dpy, colwin, RubberGC, i, ADJHEIGHT-j, ADJWIDTH-i, j);
			XDrawLine(dpy, colwin, RubberGC, ADJWIDTH-i, j, ADJWIDTH, 0);
		}
		else
			XDrawLine(dpy, colwin, RubberGC, i, ADJHEIGHT-j, ADJWIDTH, 0);
	}
	else {
		d = (double)(numcolors - start - 1);
		for (i=0; i<ADJWIDTH; i++) {
			j = get_hist_index((double)(i)/(double)ADJWIDTH);
			XDrawPoint(dpy,colwin,RubberGC,i,ADJHEIGHT-(j*ADJHEIGHT/numcolors));
		}
	}
}

Raise_Maps()
{
	extern double A, B, C, D;
	extern int cornerbar, numattrs;
	extern void init_color(), Clear();

	if (precrit && (precrit != 3) && (!(precrit > 6))) {
		XMapRaised(dpy, prejec);
		stacktop = prejec;
		if (!usedefault)
		    init_color(dpy, prejec, cmap, Colors, STARTCOLOR, MINCOLINDEX,
					numcolors, numwheels, "endo", "Endo", numattrs);
		pdown=0;
		Clear(prejec);
	}
	if (critical) {
		XMapRaised(dpy, crijec);
		stacktop = crijec;
		if (!usedefault)
		    init_color(dpy, crijec, cmap, Colors, STARTCOLOR, MINCOLINDEX,
					numcolors, numwheels, "endo", "Endo", numattrs);
		cdown=0;
		Clear(crijec);
	}
	if (lyap >= 2) {
		XMapRaised(dpy, lyajec);
		stacktop = lyajec;
		if (!usedefault)
		    init_color(dpy, lyajec, cmap, Colors, STARTCOLOR, MINCOLINDEX,
					numcolors, numwheels, "endo", "Endo", numattrs);
		ldown=0;
		Clear(lyajec);
		which = lyajec;
	}
	if (lyap == 0) {
		XMapRaised(dpy, canvas);
		stacktop = canvas;
		if (!usedefault)
		    init_color(dpy, canvas, cmap, Colors, STARTCOLOR, MINCOLINDEX,
					numcolors, numwheels, "endo", "Endo", numattrs);
		adown = 0;
		Clear(canvas);
		which = canvas;
	}
	if (showbar == 1) {
		XMapRaised(dpy, hisbar);
		if (!usedefault)
		    init_color(dpy, hisbar, cmap, Colors, STARTCOLOR, MINCOLINDEX,
					numcolors, numwheels, "endo", "Endo", numattrs);
		fillbar(hisbar, HISBARHGT, HISBARHGT, cornerbar);
	}
	else if (showbar == 2)
		fillbar(trajec, traheight/2, traheight, cornerbar);
	if (adjcol) {
		XMapRaised(dpy, colwin);
		if (!usedefault)
		    init_color(dpy, colwin, cmap, Colors, STARTCOLOR, MINCOLINDEX,
					numcolors, numwheels, "endo", "Endo", numattrs);
		drawmap(A, B, C, D);
	}
	if (portrait) {
		XMapRaised(dpy, trajec);
		stacktop = trajec;
		if (!usedefault)
		    init_color(dpy, trajec, cmap, Colors, STARTCOLOR, MINCOLINDEX,
					numcolors, numwheels, "endo", "Endo", numattrs);
		tdown=0;
		Clear(trajec);
	}
}

int xmargin, ymargin;

Title_Page()
{
	extern void Save_Parms(), init_color(), Clear();
	extern int numattrs;

	if (XDisplayWidth(dpy, screen) < 800) {
		xmargin = (XDisplayWidth(dpy, screen) - DEMO_16_WIDTH) / 2;
		ymargin = (XDisplayHeight(dpy, screen) - DEMO_16_WIDTH) / 2;
	}
	else {
		xmargin = (XDisplayWidth(dpy, screen) - DEMO_WIDTH) / 2;
		ymargin = (XDisplayHeight(dpy, screen) - DEMO_WIDTH) / 2;
	}
	XStoreName(dpy, trajec, TITLE_NAME);
	XMapRaised(dpy, trajec);
	numwheels = MAXWHEELS;
	if (!usedefault)
	    init_color(dpy, trajec, cmap, Colors, STARTCOLOR, MINCOLINDEX,
					numcolors, numwheels, "endo", "Endo", numattrs);
	stacktop = trajec;
	tdown=0;
	Clear(trajec);
	for(;;)
		if (Tit_Event())
			break;
	Clear(trajec);
	Save_Parms(0);
	XStoreName(dpy, trajec, SELECT_NAME);
	mapindex = 10 + MAP_OFF;
	Show_Maps(mapindex);
	for(;;)
		if (MapSelect())
			break;
	XUnmapWindow(dpy, lyajec); ldown=1;
	XUnmapWindow(dpy, trajec); tdown=1;
	xmargin = ymargin = 0;
}
