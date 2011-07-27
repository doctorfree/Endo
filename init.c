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

#include <stdio.h>
#include <string.h>
#include "x.h"
#ifdef USE_3D
#include "globals.h"
#endif

double window_distance = 10.0;
int lowrange, start;

void
init_data()
{
	static int i;
	extern image_data_t rubber_data;
	extern triple window_center;
	extern int Qflag, p_inc, q_inc, mandel, color_offset;

	numcolors = XDisplayCells(dpy, XDefaultScreen(dpy));
	displayplanes = DisplayPlanes(dpy, XDefaultScreen(dpy));
	if (numcolors > MAXCOLOR)
		numcolors = MAXCOLOR;
	if (numcolors <= 16) {
		STARTCOLOR = 3; MINCOLINDEX = 6;
		color_offset = 1;
	}
	numfreecols = numcolors - MINCOLINDEX;
	lowrange = MINCOLINDEX - STARTCOLOR;
	if ((lyap) || (mandel && find))
		start = MINCOLINDEX;
	else
		start = STARTCOLOR;
	if (Qflag == 4) {
		x_inc = x_range / 4.0;
		y_inc = y_range / 4.0;
		numrows = row = 4;
		p_inc = width / 4;
		q_inc = height / 4;
	}
	else {
		x_inc = x_range / (double)width;
		y_inc = y_range / (double)height;
		p_inc = 1;
		q_inc = 1;
	}
	point.x = 0;
	point.y = 0;
	row = 1; numrows = 1;
	maxexp = minexp = 0;
	x = rubber_data.p_min = min_x;
	y = rubber_data.q_min = min_y;
	rubber_data.p_max = max_x;
	rubber_data.q_max = max_y;
	if (show)
		show_defaults();
	srand48((long)time(0));
	AllInitBuffer();
	Reverse_GC = XCreateGC(dpy, DefaultRootWindow(dpy),
		(unsigned long) 0, (XGCValues *) 0);
	/* set the background of the reverse context to white */
	XSetBackground(dpy,Reverse_GC,WhitePixel(dpy,XDefaultScreen(dpy)));
	/* set the foreground of the reverse context to black */
	XSetForeground(dpy,Reverse_GC,BlackPixel(dpy,XDefaultScreen(dpy)));
	Data_GC[0] = XCreateGC(dpy, DefaultRootWindow(dpy),
		(unsigned long) 0, (XGCValues *) 0);
	/* set the background to black */
	XSetBackground(dpy,Data_GC[0],BlackPixel(dpy,XDefaultScreen(dpy)));
	/* set the foreground of the 0th context to black */
	XSetForeground(dpy,Data_GC[0],BlackPixel(dpy,XDefaultScreen(dpy)));
	Data_GC[1] = XCreateGC(dpy, DefaultRootWindow(dpy),
		(unsigned long) 0, (XGCValues *) 0);
	/* set the background to black */
	XSetBackground(dpy,Data_GC[1],BlackPixel(dpy,XDefaultScreen(dpy)));
	/* set the foreground of the 1st context to white */
	XSetForeground(dpy,Data_GC[1],WhitePixel(dpy,XDefaultScreen(dpy)));
	for (i=2; i<MAXCOLOR; i++) {
		Data_GC[i] = XCreateGC(dpy, DefaultRootWindow(dpy),
		(unsigned long) 0, (XGCValues *) 0);
		/* set the background to black */
		XSetBackground(dpy,Data_GC[i],BlackPixel(dpy,XDefaultScreen(dpy)));
		/* set the foreground of the ith context to i */
		XSetForeground(dpy, Data_GC[i], i);
	}
#ifdef USE_3D
	/*	  Initialize these global constants for everyone's benefit */
	half_pi = M_PI / 2.0;
	two_pi = 2.0 * M_PI;
	radians_per_degree = M_PI / 180.0;
	degrees_per_radian = 180.0 / M_PI;
	/*	Position the window on the z-axis */
	window_center.x = window_center.y = 0.0;
	window_center.z = WINDOW_DISTANCE;
	vue_init(window_center, trawidth, traheight);
#endif
}
