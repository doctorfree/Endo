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

#include <values.h>
#include "x.h"
#ifdef USE_3D
#include "globals.h"
#endif

void 
redisplay(event)
XExposeEvent	*event;
{
	Pixmap pix;
	extern int cornerbar;
	extern double A, B, C, D;
	extern void fillbar(), drawmap();

	if (event->window == canvas)
		pix = pixmap;
	else if (event->window == lyajec)
		pix = pixmap;
	else if (event->window == trajec)
		pix = pixtra;
	else if (event->window == crijec)
		pix = pixcri;
	else if (event->window == prejec)
		pix = pixpre;
	else if (event->window == info) {
		Show_Info();
		return;
	}
	else if (event->window == help) {
		print_help();
		return;
	}
	else if (event->window == hisbar) {
		fillbar(hisbar, HISBARHGT, HISBARHGT, cornerbar);
		return;
	}
	else if (event->window == colwin) {
		XFillRectangle(dpy, colwin, Data_GC[0], 0, 0, ADJWIDTH, ADJHEIGHT);
		drawmap(A, B, C, D);
		return;
	}
	else {
		perror("No defined window");
		Cleanup();
		exit(1);
	}
	/*
 	* Extract the exposed area from the event and copy
 	* from the saved pixmap to the window.
 	*/
	XCopyArea(dpy, pix, event->window, Data_GC[0], 
	   event->x, event->y, event->width, event->height, 
	   event->x, event->y);
	if ((pix == pixtra) && (showbar == 2))
		fillbar(trajec, traheight/2, traheight, cornerbar);
}

void
resize(w)
Window	  w;
{
	Window r;
	static int n, x, y;
	static XWindowAttributes attr;
	static XWindowChanges values;
	unsigned int new_w, new_h, bw, d;
	extern int Qflag, p_inc, q_inc;
	extern void Clear(), Redraw(); 
	extern void FreeHist(), HistoMem(), FreeCrit(), CritiMem();
	extern image_data_t rubber_data;

	XGetGeometry(dpy,w,&r,&x,&y,&new_w,&new_h,&bw,&d);
	if ((w == canvas) || (w == lyajec) || (w == prejec)){
		if ((new_w == width) && (new_h == height))
			return;
		freemem();
		width = new_w; height = new_h;
		XClearWindow(dpy,w);
		if (pixmap)
			XFreePixmap(dpy, pixmap);
		pixmap = XCreatePixmap(dpy, DefaultRootWindow(dpy), 
			width, height, DefaultDepth(dpy, screen));
		if (pixpre)
			XFreePixmap(dpy, pixpre);
		pixpre = XCreatePixmap(dpy, DefaultRootWindow(dpy), 
			width, height, DefaultDepth(dpy, screen));
		x_inc = x_range / (double)width;
		y_inc = y_range / (double)height;
		point.x = 0;
		point.y = 0;
		maxexp = minexp = 0;
		run = 1;
		x = rubber_data.p_min = min_x;
		y = rubber_data.q_min = min_y;
		row = 1; numrows = 1;
		if (Qflag == 4) {
			x_inc = x_range / 4.0;
			y_inc = y_range / 4.0;
			p_inc = width / 4;
			q_inc = height / 4;
			numrows = row = 4;
		}
		rubber_data.p_max = max_x;
		rubber_data.q_max = max_y;
		for (n=0;n<MAXFRAMES;n++)
			if ((n <= maxframe) && (n != frame))
		  		resized[n] = 1;
		setupmem();
		AllInitBuffer();
		Clear(w); Clear(trajec); Clear(crijec); Clear(prejec);
		values.width = width;
		values.height = height;
		if (w == canvas)
			XConfigureWindow(dpy,lyajec,CWWidth|CWHeight,&values);
		else
			XConfigureWindow(dpy,canvas,CWWidth|CWHeight,&values);
		XConfigureWindow(dpy,prejec,CWWidth|CWHeight,&values);
		XGetWindowAttributes(dpy, info, &attr);
		if (attr.map_state != IsUnmapped)
			Show_Info();
		Redraw();
	}
	else if (w == trajec) {
		if ((new_w == trawidth) && (new_h == traheight))
			return;
		if (histogram)
			FreeHist();
		trawidth = new_w; traheight = new_h;
		if (histogram)
			HistoMem();
		XClearWindow(dpy,trajec);
		if (pixtra)
			XFreePixmap(dpy, pixtra);
		pixtra = XCreatePixmap(dpy, DefaultRootWindow(dpy), 
			trawidth, traheight, DefaultDepth(dpy, screen));
#ifdef USE_3D
		screen_max.x = trawidth;
		screen_max.y = traheight;
		screen_center.x = trawidth >> 1;
		screen_center.y = traheight >> 1;
#endif
		Clear(trajec);
	}
	else if (w == crijec) {
		if ((new_w == criwidth) && (new_h == criheight))
			return;
		FreeCrit();
		criwidth = new_w; criheight = new_h;
		CritiMem();
		XClearWindow(dpy,crijec);
		if (pixcri)
			XFreePixmap(dpy, pixcri);
		pixcri = XCreatePixmap(dpy, DefaultRootWindow(dpy), 
			criwidth, criheight, DefaultDepth(dpy, screen));
		Clear(crijec);
		if (critical) Remap_crit(crijec);
	}
	else {
		perror("Attempt to resize unknown window");
		Cleanup();
		exit(-2);
	}
}

void
redraw(indexarray, index, cont)
int *indexarray;
int index, cont;
{
	static int i;
	static int x_sav, y_sav;

	x_sav = point.x;
	y_sav = point.y;

	point.x = 0;
	point.y = 0;

	save=0;
	for (i=0;i<index;i++) {
  		BufferPoint(dpy,which,pixmap,Data_GC,&Points,
					indexarray[i],point.x,height-point.y-1);
		if (++point.x >= width) {
			point.x = 0;
			point.y++;
		}
	}
	save=1;
	
	if (cont) {
		point.x = x_sav;
		point.y = y_sav;
	}
	else {
		x = point.x * x_inc + min_x;
		y = point.y * y_inc + min_y;
	}
	AllFlushBuffer();
}

void
Redraw() 
{
	static int i, j;
	extern int **histarray;
	extern int found_arc, Qflag, q_inc, p_inc, minhist, maxhist;

	AllFlushBuffer();
	point.x = 0;
	point.y = 0;
	minexp = maxexp = 0;
	run = 1;
	x = min_x;
	y = min_y;
	row = 1; numrows = 1;
	if (Qflag == 4) {
		x_inc = x_range / 4.0;
		y_inc = y_range / 4.0;
		p_inc = width / 4;
		q_inc = height / 4;
		numrows = row = 4;
	}
	found_arc = numcrits = numarcs = perind[frame] = 0;
	resized[frame] = 0;
	if (histogram) {
		maxhist = 0;
		minhist = MAXINT;
		for (i=0;i<traheight;i++)
			for (j=0;j<trawidth;j++)
				histarray[j][i] = 0;
	}
	Show_Info();
}

remap_crit()
{
	static int i, color=MAXCOLOR;
	static pair coords;
	extern void Clear();

	Clear(crijec);
	if (++color >= numcolors)
		color = STARTCOLOR;
	for (i=0;i<numcrits;i++) {
		draw_critical(crijec, n_crit_pts[0][i],n_crit_pts[1][i],color);
		coords=(*map)(n_crit_pts[0][i],n_crit_pts[1][i],params);
		n_crit_pts[0][i] = coords.x;
		n_crit_pts[1][i] = coords.y;
	}
}

Remap_crit(w)
Window w;
{
	static int i, j, color;
	static pair coords;

	for (j=0;j<numcrits;j++) {
		coords.x = crit_pts[0][j];
		coords.y = crit_pts[1][j];
		n_crit_pts[0][j] = coords.x;
		n_crit_pts[1][j] = coords.y;
		color = 1;
		for (i=0;i<settle+dwell;i++) {
			draw_critical(w, coords.x,coords.y,color);
			coords=(*map)(coords.x,coords.y,params);
			if (++color >= STARTCOLOR)
				color = 1;
		}
	}
}

remap_arcs()
{
	static int i;
	static pair coords;
	extern void Clear();

	Clear(crijec);
	for (i=0;i<numarcs;i++) {
		draw_critical(crijec, n_crit_arc[0][i],n_crit_arc[1][i],1);
		coords=(*map)(n_crit_arc[0][i],n_crit_arc[1][i],params);
		n_crit_arc[0][i] = coords.x;
		n_crit_arc[1][i] = coords.y;
	}
}

Remap_arcs(w)
Window w;
{
	static int i, j, color;
	static pair coords;

	for (j=0;j<numarcs;j++) {
		coords.x = crit_arc[0][j];
		coords.y = crit_arc[1][j];
		n_crit_arc[0][j] = coords.x;
		n_crit_arc[1][j] = coords.y;
		color = 1;
		for (i=0;i<settle+dwell;i++) {
			draw_critical(w, coords.x, coords.y, color);
			coords=(*map)(coords.x, coords.y, params);
			if (++color >= STARTCOLOR)
				color = 1;
		}
	}
}
