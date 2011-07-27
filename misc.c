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

#ifndef NeXT
#include <values.h>
#else
#include <math.h>
#endif
#include "x.h"

Cycle_frames()
{
	static int i;
	extern void redraw();

	for (i=0;i<=maxframe;i++)
		redraw(indices[i], perind[i], 1); 
}

SetFocus(w)
Window w;
{
	extern image_data_t rubber_data;

	stacktop = w;
	CreateXorGC(w);
	if (w == trajec) {
		rubber_data.p_min = t_min_x;
		rubber_data.q_min = t_min_y;
		rubber_data.p_max = t_min_x + t_x_range;
		rubber_data.q_max = t_min_y + t_y_range;
	}
	else if (w == crijec) {
		rubber_data.p_min = c_min_x;
		rubber_data.q_min = c_min_y;
		rubber_data.p_max = c_min_x + c_x_range;
		rubber_data.q_max = c_min_y + c_y_range;
	}
	else {
		rubber_data.p_min = min_x;
		rubber_data.q_min = min_y;
		rubber_data.p_max = min_x + x_range;
		rubber_data.q_max = min_y + y_range;
	}
}

void
flood(sea, up, old, new) /* sea level change -> sea=1; */
int sea, up, old, new;	/* value increased -> up=1 */
{						/* old := previous histogram value; ditto new */
	static int i, j, diff;
	static double power, ncols;
	extern int **histarray;
	extern int maxhist, minhist, start, cornerbar, updt;
	extern int get_hist_index();
	extern void updtbar();

	if (histogram) {
		if ((sea && (!up)) || ((!sea) && up)) {
			/* sea level went down or sky level went up */
			if ((maxhist != 0) && (minhist != MAXINT)) {
				if (maxhist == minhist)
					diff = 1;
				else
					diff = maxhist - minhist;
			}
			else
				return;
			ncols = (double)(numcolors - start - 1);
		}
		for (i=0;i<trawidth;i++)		/* redraw only those histogram */
			for (j=0;j<traheight;j++) { /* values that may have changed */
				if (sea && up) { /* sea level went up */
					if ((histarray[i][j] < new) && (histarray[i][j] >= old))
						BufferHisto(i, j, 0);
				}
				else if (sea && (!up)) { /* sea level went down */
					if ((histarray[i][j] >= new) && (histarray[i][j] < old)) {
						power = (double)(histarray[i][j]-minhist)/(double)diff;
						BufferHisto(i, j, get_hist_index(power));
					}
				}
				else if ((!sea) && up) { /* sky level went up */
					if ((histarray[i][j] <= new) && (histarray[i][j] > old)) {
						power = (double)(histarray[i][j]-minhist)/(double)diff;
						BufferHisto(i, j, get_hist_index(power));
					}
				}
				else if ((!sea) && (!up)) { /* sky level went down */
					if ((histarray[i][j] > new) && (histarray[i][j] <= old))
						BufferHisto(i, j, 0);
				}
			}
		if (updt) {
		  if (showbar == 1) {
			if (portrait && histogram)
			  updtbar(hisbar,HISBARHGT,HISBARHGT,cornerbar,maxhist,minhist);
			else
			  updtbar(hisbar,HISBARHGT,HISBARHGT,cornerbar,maxperiod,minperiod);
		  }
		  else if (showbar == 2) {
			if (portrait && histogram)
			  updtbar(trajec,traheight/2,traheight,cornerbar,maxhist,minhist);
			else
			  updtbar(trajec,traheight/2,traheight,cornerbar,maxperiod,
							minperiod);
		  }
		}
	}
}

void
rebasin() 
{
	static int i, j, k, range;
	static int diff[MAXATTRS], maxperiod[MAXATTRS], minperiod[MAXATTRS];
	static double ratio;
	extern int numattrs, first, second, begin, middle, mandel, lowrange;

	if (find) {
		range = numfreecols / numattrs;
		for (k=0; k<numattrs; k++)
			maxperiod[k] = 0; minperiod[k] = MAXINT;
		for (i=0;i<perind[frame];i++) {
			k = basins[frame][i];
			j = ABS(periods[frame][i]);
			if (k && j && (j < minperiod[k]))
				minperiod[k] = j;
			if (k && (j > maxperiod[k]))
				maxperiod[k] = j;
		}
		for (k=0; k<numattrs; k++)
			diff[k] = maxperiod[k] - minperiod[k];
		for (i=0;i<perind[frame];i++) {
	  		j = ABS(periods[frame][i]);
	  		k = basins[frame][i];
	  		if (j && diff[k]) {
				ratio=(double)(j - minperiod[k])/(double)diff[k];
				if (k) {
					if (precrit == 3) {
					  if (periods[frame][i] > 0)
						indices[frame][i] = (ratio*(second-1)) + middle;
					  else
						indices[frame][i] = (ratio*(first-1)) + begin;
					}
					else {
					   indices[frame][i]=(ratio*range)+((k-1)*range)+STARTCOLOR;
					}
				}
				else
					if (mandel == 4)
						indices[frame][i] = (ratio*(lowrange-1)) + STARTCOLOR;
					else
						indices[frame][i] = 0;
	  		}
		}
   }
}

void
rehist() 
{
	static int i, j, diff;
	static double power, ncols;
	extern int maxhist, minhist, start;
	extern int **histarray;
	extern int get_hist_index();

	if (histogram) {
		if ((maxhist != 0) && (minhist != MAXINT)) {
			if (maxhist == minhist)
				diff = 1;
			else
				diff = maxhist - minhist;
			ncols = (double)(numcolors - start - 1);
			for (i=0;i<trawidth;i++)
		  		for (j=0;j<traheight;j++) {
					if (histarray[i][j]) {
			  			power = (double)(histarray[i][j]-minhist)/(double)diff;
			  			BufferHisto(i, j, get_hist_index(power));
					}
		  		}
		}
	}
}

void
recalc() 
{
	static int i, j, diff, ncols;
	static double ratio;
	extern int lowrange, first, begin, second, middle;
	extern int start;
	
	if (lyap) {
		for (i=0;i<perind[frame];i++) {
			if (indices[frame][i] < MINCOLINDEX) {
				ratio=(double)periods[frame][i] / maxexp;
				indices[frame][i] = ((int)(ratio)%(lowrange-1))+STARTCOLOR;
			}
			else {
				ratio=(double)periods[frame][i] / (-1.0 * minexp);
				indices[frame][i]=((int)ratio%(numfreecols-1))+MINCOLINDEX;
			}
		}
	}
	else {
		maxperiod = 0; minperiod = MAXINT;
		for (i=0;i<perind[frame];i++) {
		  if ((periods[frame][i] > 0) || (find && (!basins[frame][i]))) {
			if (ABS(periods[frame][i]) < minperiod)
				minperiod = ABS(periods[frame][i]);
			if (ABS(periods[frame][i]) > maxperiod)
				maxperiod = ABS(periods[frame][i]);
		  }
		}
		diff = maxperiod - minperiod;
		for (i=0;i<perind[frame];i++) {
			if (periods[frame][i] && diff) {
		  		ratio=(double)(ABS(periods[frame][i])-minperiod)/(double)diff;
		  		if (precrit == 3) {
					if (periods[frame][i] > 0)
			  			indices[frame][i] = (ratio*(second-1)) + middle;
					else
					  if (basins[frame][i])
			  			indices[frame][i] = (ratio*(first-1)) + begin;
		  		}
		  		else
					if (periods[frame][i] > 0)
						indices[frame][i] = (ratio*(second-1)) + middle;
			}
		}
	}
}

void
Cleanup() {
	freemem();
	XCloseDisplay(dpy);
}

void
Clear(w)
Window w;
{
	static int wid, hei;
	static Pixmap p;
	extern int xmargin, ymargin, axes;
	extern void Draw_Axes();

	if (w == canvas) {
		wid = width;
		hei = height;
		p = pixmap;
	}
	else if (w == trajec) {
		if (xmargin && ymargin) {
			wid = trawidth + 1;
			hei = traheight + 1;
		}
		else {
			wid = trawidth;
			hei = traheight;
		}
		p = pixtra;
	}
	else if (w == crijec) {
		wid = criwidth;
		hei = criheight;
		p = pixcri;
	}
	else if (w == prejec) {
		wid = width;
		hei = height;
		p = pixpre;
	}
	else if (w == lyajec) {
		wid = width;
		hei = height;
		p = pixmap;
	}
	else if ((w == info) || (w == help))
		return;
	else {
		perror("Attempt to clear unknown window");
		Cleanup();
		exit(-2);\
	}
	XFillRectangle(dpy, p, Data_GC[0], xmargin, ymargin, wid, hei);
	XCopyArea(dpy,p,w,Data_GC[0],xmargin,ymargin,wid,hei,xmargin,ymargin);
	AllInitBuffer();
	if (w == trajec) {
		if (axes)
			Draw_Axes(trajec, 1);
	}
}

void
CreateXorGC(w)
Window w;
{
	XGCValues values;

	if (RubberGC)
		XFreeGC(dpy, RubberGC);
	values.foreground = foreground;
	values.line_style = LineSolid;
	values.function = GXxor;
	RubberGC = XCreateGC(dpy, DefaultRootWindow(dpy),
		  GCForeground | GCBackground | GCFunction | GCLineStyle, &values);
}

