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
#include <values.h>
#include "x.h"
#ifdef USE_3D
#include "globals.h"
#endif

static int x_str, y_str;
static int toggle=0, rubberband=0, winrot=0, segments=0;
static char xystr[40];
extern void Clear(), drawmap();

void
drawhis()	/* using this color mapping, how many points are in each color ? */
{
	static int i, j, k, minh, maxh;
	static int h[MAXCOLOR];
	static double d;
	extern int minhist, maxhist, start;
	extern int **histarray;

	XClearWindow(dpy, colwin);
	for (i=0; i<MAXCOLOR; i++)
		h[i] = 0;
	minh = MAXINT; maxh = 0;
	for (i=0; i<traheight; i++)
		for (j=0; j<trawidth; j++)
			if ((histarray[j][i]) && (maxhist > minhist)) {
				d = (double)(histarray[j][i]-minhist)/(double)(maxhist-minhist);
				k = get_hist_index(d);
				h[k]++;
				if (h[k] > maxh) maxh = h[k];
				if (h[k] < minh) minh = h[k];
			}
	k = ADJWIDTH / MAXCOLOR;
	for (i=0; i<ADJWIDTH; i+=k) {
		j = ADJHEIGHT * (h[(i/k)] - minh) / (maxh - minh);
		XFillRectangle(dpy, colwin, Data_GC[(i/k)], i, ADJHEIGHT-j, k, j);
	}
}

void 
StartSegments(w, data, event)
Window w;
image_data_t *data;
XButtonEvent *event;
{
	extern double A, B, C, D;
	extern int histex;
	static int hiscur=0;

	segments = 0;
	switch (event->button) {
		case Button1:
			if ((histex != -2) || (event->x < ADJWIDTH/2)) {
				segments = 1;
				data->rubber_band.last_x = event->x;
				data->rubber_band.last_y = ADJHEIGHT - event->y;
				drawmap(A, B, C, D);	/* erase */
				if ((histex == 0) && (event->x > ((A+C)/2)*ADJWIDTH)) {
					segments = 2;
					drawmap(A,B,
						(double)data->rubber_band.last_x/(double)ADJWIDTH,
						(double)data->rubber_band.last_y/(double)ADJHEIGHT);
				}
				else
					drawmap((double)data->rubber_band.last_x/(double)ADJWIDTH,
						(double)data->rubber_band.last_y/(double)ADJHEIGHT,C,D);
			}
			break;
		case Button2:
			drawmap(A, B, C, D);
			A = B = 0.5;
			drawmap(A, B, C, D);
			break;
		case Button3:
			hiscur = (!hiscur);
			if (hiscur) {
				drawmap(A, B, C, D);
				drawhis();
			}
			else {
				XClearWindow(dpy, colwin);
				drawmap(A, B, C, D);
			}
			break;
	}
}

int
checkwin(x, y)
int x, y;
{
	if (((x>0) && (x<ADJWIDTH)) && ((y>0) && (y<ADJHEIGHT)))
		return(1);
	else
		return(0);
}

void 
TrackSegments(w, data, event)
Window w;
image_data_t *data;
XButtonEvent *event;
{
	extern int histex;
	extern double A, B, C, D;

	if (segments) {
		if (checkwin(data->rubber_band.last_x, data->rubber_band.last_y)) {
			/* erase old lines */
			if (segments == 2)
				drawmap(A,B,(double)data->rubber_band.last_x/(double)ADJWIDTH,
						(double)data->rubber_band.last_y/(double)ADJHEIGHT);
			else
				drawmap((double)data->rubber_band.last_x/(double)ADJWIDTH,
						(double)data->rubber_band.last_y/(double)ADJHEIGHT,C,D);
		}
		if ((histex != -2) || (event->x < ADJWIDTH/2)) {
			data->rubber_band.last_x = event->x;
			data->rubber_band.last_y = ADJHEIGHT - event->y;
			if (checkwin(data->rubber_band.last_x, data->rubber_band.last_y)) {
			  if (segments == 2)
				drawmap(A, B, (double)event->x/(double)ADJWIDTH,
						((double)data->rubber_band.last_y/(double)ADJHEIGHT));
			  else
				drawmap((double)event->x/(double)ADJWIDTH,
						(double)data->rubber_band.last_y/(double)ADJHEIGHT,C,D);
			}
		}
		else {
			if (checkwin(data->rubber_band.last_x, data->rubber_band.last_y)) {
				drawmap((double)data->rubber_band.last_x/(double)ADJWIDTH,
						(double)data->rubber_band.last_y/(double)ADJHEIGHT,C,D);
			}
		}
	}
}

void 
EndSegments(w, data, event)
Window w;
image_data_t *data;
XButtonEvent *event;
{
	extern double A, B, C, D;
	extern void Redraw_Hist();

	if (segments) {
		if (checkwin(data->rubber_band.last_x, data->rubber_band.last_y)) {
			if (segments == 2) {
				C = (double)data->rubber_band.last_x/(double)ADJWIDTH;
				D = (double)data->rubber_band.last_y/(double)ADJHEIGHT;
			}
			else {
				A = (double)data->rubber_band.last_x/(double)ADJWIDTH;
				B = (double)data->rubber_band.last_y/(double)ADJHEIGHT;
			}
			Redraw_Hist();
		}
		else
			drawmap(A, B, C, D);
		segments = 0;
	}
}

void 
StartRubberBand(w, data, event)
Window w;
image_data_t *data;
XButtonEvent *event;
{
	XPoint corners[5];
	double x, y;
	static int wid, hei;
	extern void SetupCorners();

	nostart = 0;
	rubberband = 0;
	winrot = 0;
	switch (event->button) {
		case Button1:
			if (clearflag) {
				clearflag=0;
				toggle=1;
			}
			rubberband=1;
			data->rubber_band.last_x = data->rubber_band.start_x = event->x;
			data->rubber_band.last_y = data->rubber_band.start_y = event->y;
			SetupCorners(corners, data);
			XDrawLines(dpy, w, RubberGC, corners, 
					sizeof(corners) / sizeof(corners[0]), CoordModeOrigin);
			break;
#ifdef USE_3D
	  case Button2:
			winrot = 1;
			if (w == trajec) {
				wid = trawidth/2;
				hei = traheight/2;
			}
			else if (w == crijec) {
				wid = criwidth/2;
				hei = criheight/2;
			}
			else if (w == prejec) {
				wid = width/2;
				hei = height/2;
			}
			else {
				wid = width/2;
				hei = height/2;
			}
			x = ((double)(event->x - wid) / (double)wid) * 180.0;
			y = ((double)(event->y - hei) / (double)hei) * 180.0;
			sprintf(xystr," (%lf,%lf) ",x,y);
			x_str = (wid/2);
			y_str = (2*hei) - 20;
			XDrawImageString(dpy,w,Data_GC[1],x_str,y_str,xystr,strlen(xystr));
			break;
	  case Button3:
			winrot = 2;
			x_str = event->x;
			y_str = event->y;
			break;
#endif
	}
}

void
SetupCorners(corners, data)
XPoint *corners;
image_data_t *data;
{
	corners[0].x = data->rubber_band.start_x;
	corners[0].y = data->rubber_band.start_y;
	corners[1].x = data->rubber_band.start_x;
	corners[1].y = data->rubber_band.last_y;
	corners[2].x = data->rubber_band.last_x;
	corners[2].y = data->rubber_band.last_y;
	corners[3].x = data->rubber_band.last_x;
	corners[3].y = data->rubber_band.start_y;
	corners[4] = corners[0];
}

void 
TrackRubberBand(w, data, event)
Window w;
image_data_t *data;
XButtonEvent *event;
{
	XPoint corners[5];
	int xdiff, ydiff;
	double x, y, z;
	static int wid, hei;
	extern void SetupCorners(); 
	extern int axes;
#ifdef USE_3D
	extern void TranslateWindow(), Draw_Axes();
	extern triple window_center;
	static triple tvec;
#endif

	if (rubberband) {
		if (nostart)
			return;
		SetupCorners(corners, data);
		XDrawLines(dpy, w, RubberGC,
			corners, sizeof(corners) / sizeof(corners[0]), CoordModeOrigin);
		ydiff = event->y - data->rubber_band.start_y;
		xdiff = event->x - data->rubber_band.start_x;
		data->rubber_band.last_x = data->rubber_band.start_x + xdiff;
		data->rubber_band.last_y = data->rubber_band.start_y + ydiff;
		if (data->rubber_band.last_y < data->rubber_band.start_y ||
			data->rubber_band.last_x < data->rubber_band.start_x) {
			data->rubber_band.last_y = data->rubber_band.start_y;
			data->rubber_band.last_x = data->rubber_band.start_x;
		}
		SetupCorners(corners, data);
		XDrawLines(dpy, w, RubberGC,
			corners, sizeof(corners) / sizeof(corners[0]), CoordModeOrigin);
	}
#ifdef USE_3D
	else if (winrot) {
		if (w == trajec) {
			wid = trawidth/2;
			hei = traheight/2;
		}
		else if (w == crijec) {
			wid = criwidth/2;
			hei = criheight/2;
		}
		else if (w == prejec) {
			wid = width/2;
			hei = height/2;
		}
		else {
			wid = width/2;
			hei = height/2;
		}
		if (winrot == 1) {
			x = ((double)(event->x - wid) / (double)wid) * 180.0;
			y = ((double)(event->y - hei) / (double)hei) * 180.0;
			sprintf(xystr," (%lf,%lf) ",x,y);
			x_str = (wid/2);
			y_str = (2*hei) - 20;
			XDrawImageString(dpy,w,Data_GC[1],x_str,y_str,xystr,strlen(xystr));
		}
		else {
			if ((ABS(event->x - x_str)>(wid/20)) || 
				(ABS(event->y - y_str)>(hei/20))) {
				VECTORIZE(tvec, window_right, window_center);
				z = (double)(event->x - x_str) / (double)wid;
				VECTOR_X_SCALAR(tvec, tvec, z);
				if (axes)
					Draw_Axes(w, 0);
				TranslateWindow(tvec);
				VECTORIZE(tvec, window_top, window_center);
				z = (double)(event->y - y_str) / (double)hei;
				VECTOR_X_SCALAR(tvec, tvec, z);
				TranslateWindow(tvec);
				x_str = event->x; y_str = event->y;
				Clear(w);
				if (axes)
				Draw_Axes(w, 1);
	  		}
		}
	}
	else {
#endif
		x = (((double)event->x / (double)width) * x_range) + min_x;
		y = (((double)(height-event->y) / (double)height) * y_range) + min_y;
		sprintf(xystr," (%lf,%lf) ",x,y);
		XDrawImageString(dpy,w,Data_GC[1],x_str,y_str,xystr,
					strlen(xystr));
#ifdef USE_3D
	}
#endif
}

void 
EndRubberBand(w, data, event)
Window w;
image_data_t *data;
XButtonEvent *event;
{
	XPoint corners[5];
	XPoint top, bot;
	double del, diff, xeven, yeven;
	static int wid, hei;
	extern int axes;
#ifdef USE_3D
	static triple rotate_axis;
	extern void TranslateWindow(), Draw_Axes();
#endif
	extern void set_new_params(), SetupCorners(); 

	switch (event->button) {
		case Button1:
			nostart = 1;
			rubberband = 1;
			SetupCorners(corners, data);
			XDrawLines(dpy, w, RubberGC,
				corners, sizeof(corners) / sizeof(corners[0]), CoordModeOrigin);
			if (data->rubber_band.start_x >= data->rubber_band.last_x ||
				data->rubber_band.start_y >= data->rubber_band.last_y)
				return;
			top.x = data->rubber_band.start_x;
			bot.x = data->rubber_band.last_x;
			top.y = data->rubber_band.start_y;
			bot.y = data->rubber_band.last_y;
			diff = data->q_max - data->q_min;
			if (w == trajec) {
				wid = trawidth;
				hei = traheight;
		/* Gack! Why doesn't this work ?
 		*		if (orbits_3d) {
 		*			diff = (double)top.y / (double)hei;
 		*			VECTORIZE(rotate_axis,window_upper_right,window_right);
 		*			VECTOR_X_SCALAR(rotate_axis, rotate_axis, 2.0 * diff);
 		*			TranslateWindow(rotate_axis);
 		*			VECTOR_X_SCALAR(rotate_axis, rotate_axis, -1.0);
 		*			TranslateView(rotate_axis);
 		*			del = 1.0 - ((double)bot.x / (double)wid);
 		*			VECTORIZE(rotate_axis,window_upper_right,window_top);
 		*			VECTOR_X_SCALAR(rotate_axis, rotate_axis, 2.0 * del);
 		*			TranslateWindow(rotate_axis);
 		*			VECTOR_X_SCALAR(rotate_axis, rotate_axis, -1.0);
 		*			TranslateView(rotate_axis);
 		*			del = (double)(bot.x - top.x) / (double)wid;
 		*			diff = (double)(bot.y - top.y) / (double)hei;
 		* 			ScaleWindow(del, diff);
 		*			Clear(trajec);
 		*			break;
 		*		}
 		*/
			}
			else if (w == crijec) {
				wid = criwidth;
				hei = criheight;
			}
			else if (w == prejec) {
				wid = width;
				hei = height;
			}
			else {
				wid = width;
				hei = height;
			}
			del = (double)(hei-bot.y) / (double)hei;
			data->q_min += diff * del;
			del = (double)top.y / (double)hei;
			data->q_max -= diff * del;
			diff = data->p_max - data->p_min;
			del = (double)top.x / (double)wid;
			data->p_min += diff * del;
			del = (double)(wid - bot.x) / (double)wid;
			data->p_max -= diff * del;
			fflush(stdout);
			set_new_params(w, data);
			if (toggle) {
				toggle=0;
				clearflag=1;
			}
			break;
#ifdef USE_3D
		case Button2:
			if (w == trajec) {
				wid = trawidth/2;
				hei = traheight/2;
			}
			else if (w == crijec) {
				wid = criwidth/2;
				hei = criheight/2;
			}
			else if (w == prejec) {
				wid = width/2;
				hei = height/2;
			}
			else {
				wid = width/2;
				hei = height/2;
			}
			xeven = ((double)(event->x - wid) / (double)wid) * 180.0;
			yeven = ((double)((hei-event->y) - hei) / (double)hei) * 180.0;
			VECTORIZE(rotate_axis, window_center, window_right);
			UNIT_VECTOR(rotate_axis, rotate_axis, VECTOR_LENGTH(rotate_axis));
			if (axes)
				Draw_Axes(w, 0);
			RotateWindow( & yeven, & rotate_axis );
			VECTORIZE(rotate_axis, window_center, window_top);
			UNIT_VECTOR(rotate_axis, rotate_axis, VECTOR_LENGTH(rotate_axis));
			RotateWindow( & xeven, & rotate_axis );
			Clear(w);
			if (axes)
				Draw_Axes(w, 1);
			break;
#endif
	}
}

void
set_new_params(w, data)
Window w;
image_data_t *data;
{
	static XWindowAttributes attr;
	extern int Qflag, p_inc, q_inc;

	if (w == crijec) {
		c_x_range = data->p_max - data->p_min;
		c_y_range = data->q_max - data->q_min;
		c_min_x = data->p_min;
		c_min_y = data->q_min;
		Clear(crijec);
		if (critical) Remap_crit(crijec);
	}
	else if (w == trajec) {
		t_x_range = data->p_max - data->p_min;
		t_y_range = data->q_max - data->q_min;
		t_min_x = data->p_min;
		t_min_y = data->q_min;
		Clear(trajec);
	}
	else {
		frame = (maxframe + 1) % MAXFRAMES;
		if (frame > maxframe)
			maxframe = frame;
		x_range = data->p_max - data->p_min;
		y_range = data->q_max - data->q_min;
		a_minimums[frame] = min_x = data->p_min;
		b_minimums[frame] = min_y = data->q_min;
		x_inc = x_range / (double)width;
		y_inc = y_range / (double)height;
		a_maximums[frame] = max_x = data->p_max;
		b_maximums[frame] = max_y = data->q_max;
		point.x = 0;
		point.y = 0;
		maxexp = minexp = 0;
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
		perind[frame] = 0;;
		Clear(which); Clear(prejec);
	}
	XGetWindowAttributes(dpy, info, &attr);
	if (attr.map_state != IsUnmapped)
		Show_Info();
}
