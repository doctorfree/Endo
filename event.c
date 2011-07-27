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

#include "x.h"

image_data_t rubber_data;
int p_inc, q_inc;

copy_periods()
{
	static int i, j, copyrows;

	if ((2*point.y) > height)
		copyrows = height - (point.y + 1);
	else
		copyrows = point.y - 1;
	if (lyap)
		for (i = 0; i<copyrows; i++) {
			for (j = 0; j<width; j++) {
				periods[frame][(((copyrows+point.y-1)-i)*width)+j] = 
						periods[frame][((point.y-copyrows+i)*width)+j];
				indices[frame][(((copyrows+point.y-1)-i)*width)+j] =
						indices[frame][((point.y-copyrows+i)*width)+j];
			}
		}
	else
		for (i = 0; i<copyrows; i++) {
			for (j = 0; j<width; j++) {
				periods[frame][(((copyrows+point.y)-i)*width)-(j+1)] = 
						periods[frame][((point.y-copyrows+i)*width)+j];
				indices[frame][(((copyrows+point.y)-i)*width)-(j+1)] =
						indices[frame][((point.y-copyrows+i)*width)+j];
			}
		}
	perind[frame] += copyrows * width;
	point.y += copyrows;
	y = ((point.y-1) * y_inc) + min_y;
	AllFlushBuffer(); redraw(indices[frame], perind[frame], 1);
}

increment(method)
int method;
{
	extern void save_to_file();
	extern double drand48();
	extern int symmetric[], found_arc, idown, demo;
	static int tlyap;

	if ((!method) || (method == 4)) {	/* compute row by row like normal */
		x += x_inc;			 /* or refined mesh (method = 4) */
		point.x += p_inc;
		if ((x >= max_x) || (point.x >= width)) {
			x = min_x;
			y += y_inc;
			point.x = 0;
			point.y += q_inc;
			if (clearflag)
				Clear(trajec);
			if (randinit && lyap) {
				tlyap = lyap;
				lyap = 0; setparams(mapindex);
				start_x = (drand48() * x_range) + min_x;
				start_y = (drand48() * y_range) + min_y;
				lyap = tlyap; setparams(mapindex);
			}
			if (symmetric[mapindex] && (y>0) && ((y-y_inc) <= 0))
				copy_periods();
		}
		if ((y >= max_y) || (point.y >= height)) {
			AllFlushBuffer();
			if ((found_arc) && (critical))
				find_arcs(A0, A1);
			if (method == 4) {
				x = min_x; y = min_y;
				point.x = point.y = 0;
				x_inc *= 0.5; y_inc *= 0.5; 
				p_inc *= 0.5; q_inc *= 0.5;
				if (((numrows *= 2) > width) && ((row *=2) > height)) {
					run = 0;
					if (histogram)
						FlushBuffer(dpy,trajec,pixtra,Data_GC,&Histog,
										0,numcolors);
					if (!idown)
						Show_Info();
					if (demo) {
						recalc();
						redraw(indices[frame], perind[frame], 1);
					}
					if (savefile == 1)
						save_to_file(pixmap, width, height);
					if (savefile == 2)
						save_to_file(pixtra,trawidth,traheight);
				}
			}
			else {
				run = 0;
				if (histogram)
					FlushBuffer(dpy,trajec,pixtra,Data_GC,&Histog,0,numcolors);
				if (!idown)
					Show_Info();
				if (savefile == 1)
					save_to_file(pixmap, width, height);
				if (savefile == 2)
					save_to_file(pixtra,trawidth,traheight);
			}
		}
	}
	else if (method == 1) {		/* just compute the diagonal */
		x += x_inc;
		y = x;
		point.x++;
		point.y = (y - min_y) / y_inc;
		if ((x >= max_x) || (point.x >= width) || 
			(y >= max_y) || (point.y >= height)) {
			AllFlushBuffer();
			if ((found_arc) && (critical))
				find_arcs(A0, A1);
			run = 0;
			if (histogram)
				FlushBuffer(dpy,trajec,pixtra,Data_GC,&Histog,0,numcolors);
			if (!idown)
				Show_Info();
			if (savefile == 1)
				save_to_file(pixmap, width, height);
			if (savefile == 2)
				save_to_file(pixtra,trawidth,traheight);
		}
	}
	else if (method == 2) {		/* compute a row then a column */
		if (row) {
			x += x_inc;
			point.x++;
		}
		else {
			y += y_inc;
			point.y++;
		}
		if ((x >= max_x-(numrows*x_inc))||(point.x >= width-numrows)) {
			row = 0;
			y += y_inc;
			point.y++;
			x -= x_inc;
			point.x--;
			if (clearflag)
				Clear(trajec);
			if (randinit && lyap) {
				tlyap = lyap;
				lyap = 0; setparams(mapindex);
				start_x = (drand48() * x_range) + min_x;
				start_y = (drand48() * y_range) + min_y;
				lyap = tlyap; setparams(mapindex);
			}
		}
		if ((y >= max_y) || (point.y >= height)) {
			row = 1;
			x = min_x;
			point.x = 0;
			y = min_y + (numrows * y_inc);
			point.y = numrows++;
			if ((y >= max_y) || (point.y >= height)) {
				AllFlushBuffer();
				if ((found_arc) && (critical))
					find_arcs(A0, A1);
				run = 0;
				if (histogram)
					FlushBuffer(dpy,trajec,pixtra,Data_GC,&Histog,0,numcolors);
				if (!idown)
					Show_Info();
				if (savefile == 1)
					save_to_file(pixmap, width, height);
				if (savefile == 2)
					save_to_file(pixtra,trawidth,traheight);
			}
		}
	}
	else if (method == 3) {		/* compute a column then a row */
		if (row) {
			y += y_inc;
			point.y++;
		}
		else {
			x += x_inc;
			point.x++;
		}
		if ((y >= max_y-(numrows*y_inc))||(point.y >= height-numrows)) {
			row = 0;
			x += x_inc;
			point.x++;
			y -= y_inc;
			point.y--;
			if ((y < min_y) || (point.y < 0)) {
				AllFlushBuffer();
				if ((found_arc) && (critical))
					find_arcs(A0, A1);
				run = 0;
				if (histogram)
					FlushBuffer(dpy,trajec,pixtra,Data_GC,&Histog,0,numcolors);
				if (!idown)
					Show_Info();
				if (savefile == 1)
					save_to_file(pixmap, width, height);
				if (savefile == 2)
					save_to_file(pixtra,trawidth,traheight);
			}
			if (clearflag)
				Clear(trajec);
			if (randinit && lyap) {
				tlyap = lyap;
				lyap = 0; setparams(mapindex);
				start_x = (drand48() * x_range) + min_x;
				start_y = (drand48() * y_range) + min_y;
				lyap = tlyap; setparams(mapindex);
			}
		}
		if ((x >= max_x) || (point.x >= width)) {
			row = 1;
			y = min_y;
			point.y = 0;
			x = min_x + (numrows * x_inc);
			point.x = numrows++;
			if ((x >= max_x) || (point.x >= width)) {
				AllFlushBuffer();
				if ((found_arc) && (critical))
					find_arcs(A0, A1);
				run = 0;
				if (histogram)
					FlushBuffer(dpy,trajec,pixtra,Data_GC,&Histog,0,numcolors);
				if (!idown)
					Show_Info();
				if (savefile == 1)
					save_to_file(pixmap, width, height);
				if (savefile == 2)
					save_to_file(pixtra,trawidth,traheight);
			}
		}
	}
	else
		usage();
}

main_event()
{
	static int n, p_exposed, c_exp;
	static int exposed,c_exposed,t_exposed,l_exposed,i_exposed,h_exposed,b_exp;
	XEvent event,c_event,a_event,t_event,l_event,i_event,h_event,p_event,b_eve;
	XEvent c_eve;
	extern int Qflag;
	extern void Getkey();

	if (run) {
		compendo();
		increment(Qflag);
	}
	exposed = t_exposed = l_exposed = i_exposed = h_exposed = c_exposed = 0;
	p_exposed = b_exp = c_exp = 0;
	n = XEventsQueued(dpy, QueuedAfterFlush);
	while (n--) {
		XNextEvent(dpy, &event);
		switch(event.type) 
		{
		case KeyPress:
			Getkey(&event);
			break;
		case Expose:
			if (event.xexpose.window == canvas) {
				exposed = 1;
				a_event = event;
			}
			else if (event.xexpose.window == trajec) {
				t_exposed = 1;
				t_event = event;
			}
			else if (event.xexpose.window == crijec) {
				c_exposed = 1;
				c_event = event;
			}
			else if (event.xexpose.window == prejec) {
				p_exposed = 1;
				p_event = event;
			}
			else if (event.xexpose.window == lyajec) {
				l_exposed = 1;
				l_event = event;
			}
			else if (event.xexpose.window == info) {
				i_exposed = 1;
				i_event = event;
			}
			else if (event.xexpose.window == help) {
				h_exposed = 1;
				h_event = event;
			}
			else if (event.xexpose.window == hisbar) {
				b_exp = 1;
				b_eve = event;
			}
			else if (event.xexpose.window == colwin) {
				c_exp = 1;
				c_eve = event;
			}
			break;
		case ConfigureNotify:
			resize(event.xconfigure.window);
			break;
		case ButtonPress:
			if ((event.xbutton.window == canvas) ||
				(event.xbutton.window == trajec) ||
				(event.xbutton.window == crijec) ||
				(event.xbutton.window == lyajec) ||
				(event.xbutton.window == prejec))
				StartRubberBand(event.xbutton.window, &rubber_data, &event);
			if (event.xbutton.window == colwin)
				StartSegments(event.xbutton.window, &rubber_data, &event);
			break;
		case MotionNotify:
			if ((event.xmotion.window == canvas) ||
				(event.xmotion.window == trajec) ||
				(event.xmotion.window == crijec) ||
				(event.xmotion.window == lyajec) ||
				(event.xmotion.window == prejec))
				TrackRubberBand(event.xmotion.window, &rubber_data, &event);
			if (event.xmotion.window == colwin)
				TrackSegments(event.xmotion.window, &rubber_data, &event);
			break;
		case ButtonRelease:
			if ((event.xbutton.window == canvas) ||
				(event.xbutton.window == trajec) ||
				(event.xbutton.window == crijec) ||
				(event.xbutton.window == lyajec) ||
				(event.xbutton.window == prejec))
				EndRubberBand(event.xbutton.window, &rubber_data, &event);
			if (event.xmotion.window == colwin)
				EndSegments(event.xbutton.window, &rubber_data, &event);
			break;
		case FocusIn:
			SetFocus(event.xfocus.window);
			break;
		}
	}
	if (exposed)
		redisplay(&a_event);
	if (t_exposed)
		redisplay(&t_event);
	if (c_exposed)
		redisplay(&c_event);
	if (p_exposed)
		redisplay(&p_event);
	if (l_exposed)
		redisplay(&l_event);
	if (b_exp)
		redisplay(&b_eve);
	if (c_exp)
		redisplay(&c_eve);
	if (h_exposed)
		redisplay(&h_event);
	if (i_exposed)
		redisplay(&i_event);
}

