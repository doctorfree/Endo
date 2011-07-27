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

#include <math.h>
#include <values.h>
#include "x.h"

double A=0.05, B=0.85, C=0.1, D=0.95;

int orbit_in_basin = 0, orbit_in_precrit = 0, orbit_in_crit = 0;
int see_domain = 0, histex = 0;
double sea_level = 0.0, sky_level = 1.0;
static int hcolor;

int
get_hist_index(p)
double p;
{
	extern double pow(), asin();
	extern int start;
	static int m, s;

	if ((p < sea_level) || (p > sky_level))
		return(0);
	m = numcolors - start - 1;
	s = start;
	if (histex == 0) /* three segments (0,0)-(A,mB)-(C,mD)-(1,m) */
		if (p < A)
			return((int)(m*B*p/A) + s);
		else if (p < C)
			return((int)((m*(((D-B)*(p-A)/(C-A))+B)))+ s);
		else
			return((int)((m*(1.0-D)*(p-1.0)/(1.0-C))+m) + s);
	if (histex == 1)	/* two lines (0,0)-(A,mB) and (A,mB)-(1,m) */
		if (p < A)
			return((int)(m*B*p/A) + s);
		else
			return((int)(m*(((1.0-B)/(1.0-A)*(p-1.0))+1.0)) + s);
	else if (histex == 2) /* e.g. 256^(1-p) with 0 < p < 1 */
		return((int)(pow((double)m, 1.0 - p)) + s);
	else if (histex == 3)
		return((int)((0.5 + (asin((p*2.0) - 1.0)/M_PI))*m) + s);
	else if (histex == 4) /* two lines (0,0)-(1-A,1-mB) and (1-A,1-mB)-(1,m) */
		if (p < (1.0 - A))
			return((int)(m*(1.0-B)*p/(1.0-A)) + s);
		else
			return((int)(m*((B*(p-1.0)/A)+1.0)) + s);
	else if (histex == -1) /* e.g. 256 - 256^(1-p) with 0 < p < 1 */
		return((int)(m - pow((double)m, 1.0 - p)) + s);
	else if (histex == -2) /* three segments (0,0)-(A,mB)-(1-A,1-mB)-(1,m) */
		if (p < A)
			return((int)(m*B*p/A) + s);
		else if (p < (1.0 - A))
			return((int)(((m-(2.0*m*B))*(p-A)/(1.0-(2.0*A)))+(m*B)) + s);
		else
			return((int)((m*B*(p-1.0)*A)+m) + s);
	else if (histex == -3)	/* reverse linear */
		return((int)(m * (1.0 - p)) + s);
	else					/* linear */
		return((int)(m * p) + s);
}

void
BufferHisto(x, y, n)
int x, y, n;
{
	static int xm, ym, diff;
	static double ncols, power;
	extern int xmargin, ymargin, animate, start, minhist, maxhist;
	extern int **histarray;

	if (n == -1) {
		if ((maxhist != 0) && (minhist != MAXINT)) {
			if (maxhist == minhist)
				diff = 1;
			else
				diff = maxhist - minhist;
		}
		else
			return;
		ncols = (double)(numcolors - start - 1);
		power = (double)(histarray[x][y] - minhist)/(double)diff;
		hcolor = get_hist_index(power);
	}
	else
		hcolor = n;
	xm = x+xmargin; ym = (traheight-y)+ymargin;
	XDrawPoint(dpy, trajec, Data_GC[hcolor], xm, ym);
	XDrawPoint(dpy, pixtra, Data_GC[hcolor], xm, ym);
}

void
draw_precrit(x1,y1,gc)
double x1,y1;
int gc;
{
	static int x2, y2;

	if ((x1 > min_x) && (y1 > min_y)) {
		x2 = ((x1 - min_x) / x_range) * (width);
		y2 = ((y1 - min_y) / y_range) * (height);
		if ((x2<width) && (y2<height))
			if (orbit_in_precrit)
			   BufferPoint(dpy,prejec,pixpre,Data_GC,&Basins,1,x2,height-y2-1);
			else
			   BufferPoint(dpy,prejec,pixpre,Data_GC,&Basins,gc,x2,height-y2-1);
	}
}

void
draw_critical(w,x1,y1,gc)
Window w;
double x1,y1;
int gc;
{
	static int x2, y2, wid, hei;
	static Pixmap p;
	static double minx, miny, xr, yr;

	if (w == crijec) {
		wid = criwidth;
		hei = criheight;
		p = pixcri;
		minx = c_min_x;
		miny = c_min_y;
		xr = c_x_range;
		yr = c_y_range;
	}
	else if (w == trajec) {
		wid = trawidth;
		hei = traheight;
		p = pixtra;
		minx = t_min_x;
		miny = t_min_y;
		xr = t_x_range;
		yr = t_y_range;
	}
	if ((x1 > minx) && (y1 > miny)) {
		x2 = ((x1 - minx) / xr) * (wid);
		y2 = ((y1 - miny) / yr) * (hei);
		if ((x2<wid) && (y2<hei)) {
			if (orbit_in_crit) {
				XDrawPoint(dpy,w,Data_GC[1],x2,hei-y2-1);
				XDrawPoint(dpy,p,Data_GC[1],x2,hei-y2-1);
			}
			else {
				XDrawPoint(dpy,w,Data_GC[gc],x2,hei-y2-1);
				XDrawPoint(dpy,p,Data_GC[gc],x2,hei-y2-1);
			}
		}
	}
}

int
inlevel(n) 
int n;
{
	static int m, M;
	extern int maxhist, minhist;

	if (n == 0)
		return(0);
	if ((minhist == MAXINT) || (maxhist == 0))
		return(1);
	m = Max(1, (sea_level * (maxhist - minhist)) + minhist - 1);
	M = (sky_level * (maxhist - minhist)) + minhist + 1;
	if ((n >= m) && (n <= M))
		return(1);
	else
		return(0);
}

void
draw_portrait(x1,y1,gc)
double x1,y1;
int gc;
{
	static xy_t p;
	static int bar;
	extern int xmargin, ymargin, orbits_1d, cornerbar;
#ifdef NorthSouth
	extern double O_B_zero, O_B_one, B_zero, B_one, singularity;
	extern double O_S_B_zero, O_S_B_one, S_B_zero, S_B_one;
	extern double S_singularity;
#endif
	extern int **histarray;
	extern int minhist, maxhist, updt;
#ifdef USE_3D
	extern int plot();
	extern void updtbar();
	extern double window_distance;
	triple xyz;

	if (orbits_3d && lyap) {
		xyz.x = x1;
		xyz.y = y1;
		xyz.z = (((params[p1]-min_x)/x_range) * 
			(2.0 * window_distance)) - window_distance;
		if (plot( xyz, &p));
			BufferPoint(dpy,trajec,pixtra,Data_GC,&Orbits,
							gc, p.x + xmargin, (traheight-p.y) + ymargin);
	}
	else {
#endif
		if (lyap && orbits_1d) {
			p.x=(((params[p1]-min_x)/x_range) * (double)trawidth); 
#ifdef NorthSouth
		if (mapindex == 7) {
			if ((orbits_1d < 3) && (see_domain)) {
				p.y=(int)(((B_zero-t_min_y)/t_y_range) * (double)traheight);
				if ((p.x<trawidth) && (p.y>0))
					BufferPoint(dpy,trajec,pixtra,Data_GC,&Orbits,
									2,p.x+xmargin,(traheight-p.y)+ymargin);
				p.y=(int)((((B_one+B_zero)-t_min_y)/t_y_range) * 
						(double)traheight);
				if ((p.x<trawidth) && (p.y>0))
					BufferPoint(dpy,trajec,pixtra,Data_GC,&Orbits,
									2,p.x+xmargin,(traheight-p.y)+ymargin);
				p.y=(int)(((O_B_zero-t_min_y)/t_y_range) * 
						(double)traheight);
				if ((p.x<trawidth) && (p.y>0))
					BufferPoint(dpy,trajec,pixtra,Data_GC,&Orbits,
									6,p.x+xmargin,(traheight-p.y)+ymargin);
				p.y=(int)((((O_B_one+O_B_zero)-t_min_y)/t_y_range) * 
						(double)traheight);
				if ((p.x<trawidth) && (p.y>0))
					BufferPoint(dpy,trajec,pixtra,Data_GC,&Orbits,
									6,p.x+xmargin,(traheight-p.y)+ymargin);
				p.y=(int)(((singularity-t_min_y)/t_y_range) * 
						(double)traheight);
				if ((p.x<trawidth) && (p.y>0))
					BufferPoint(dpy,trajec,pixtra,Data_GC,&Orbits,
									1,p.x+xmargin,(traheight-p.y)+ymargin);
			}
			if ((orbits_1d > 1) && (see_domain)) {
				p.y=(int)(((S_B_zero-t_min_y)/t_y_range)*(double)traheight);
				if ((p.x<trawidth) && (p.y>0))
					BufferPoint(dpy,trajec,pixtra,Data_GC,&Orbits,
									2,p.x+xmargin,(traheight-p.y)+ymargin);
				p.y=(int)((((S_B_one+S_B_zero)-t_min_y)/t_y_range) * 
						(double)traheight);
				if ((p.x<trawidth) && (p.y>0))
					BufferPoint(dpy,trajec,pixtra,Data_GC,&Orbits,
									2,p.x+xmargin,(traheight-p.y)+ymargin);
				p.y=(int)(((O_S_B_zero-t_min_y)/t_y_range) * 
						(double)traheight);
				if ((p.x<trawidth) && (p.y>0))
					BufferPoint(dpy,trajec,pixtra,Data_GC,&Orbits,
									6,p.x+xmargin,(traheight-p.y)+ymargin);
				p.y=(int)((((O_S_B_one+O_S_B_zero)-t_min_y)/t_y_range) * 
						(double)traheight);
				if ((p.x<trawidth) && (p.y>0))
					BufferPoint(dpy,trajec,pixtra,Data_GC,&Orbits,
									6,p.x+xmargin,(traheight-p.y)+ymargin);
				p.y=(int)(((S_singularity-t_min_y)/t_y_range) * 
						(double)traheight);
				if ((p.x<trawidth) && (p.y>0))
					BufferPoint(dpy,trajec,pixtra,Data_GC,&Orbits,
									1,p.x+xmargin,(traheight-p.y)+ymargin);
			}
		}
#endif /* NorthSouth */
		if (orbits_1d < 3) {
			if (x1 > t_min_x)
				p.y=(int)(((x1-t_min_x)/t_x_range) * (double)traheight);
			if ((p.x<trawidth) && (p.y<traheight))
				if (histogram) {
					bar = 0;
					histarray[p.x][p.y]++;
					if (histarray[p.x][p.y] > maxhist) {
						maxhist = histarray[p.x][p.y];
						bar = 1;
					}
					if (histarray[p.x][p.y] < minhist) {
						minhist = histarray[p.x][p.y];
						bar = 1;
					}
					if (inlevel(histarray[p.x][p.y]))
						BufferHisto(p.x, p.y,-1);
				}
				else
					BufferPoint(dpy,trajec,pixtra,Data_GC,&Orbits,
									gc,p.x+xmargin,(traheight-p.y)+ymargin);
		}
		if (orbits_1d > 1) {
			if (y1 > t_min_y)
				p.y=(int)(((y1-t_min_y)/t_y_range) * (double)traheight);
			if ((p.x<trawidth) && (p.y<traheight))
				if (histogram) {
					bar = 0;
					histarray[p.x][p.y]++;
					if (histarray[p.x][p.y] > maxhist) {
						maxhist = histarray[p.x][p.y];
						bar = 1;
					}
					if (histarray[p.x][p.y] < minhist) {
						minhist = histarray[p.x][p.y];
						bar = 1;
					}
					if (inlevel(histarray[p.x][p.y]))
						BufferHisto(p.x, p.y,-1);
				}
				else
					BufferPoint(dpy,trajec,pixtra,Data_GC,&Orbits,
									gc,p.x+xmargin,(traheight-p.y)+ymargin);
			}
		 }
		 else if (orbits_1d % 2) {
			p.x=(((x-min_x)/x_range) * (double)trawidth); 
			p.y=(int)(((x1-t_min_x)/t_x_range) * (double)traheight);
			if ((p.x<trawidth) && (p.y<traheight))
				if (histogram) {
					bar = 0;
					histarray[p.x][p.y]++;
					if (histarray[p.x][p.y] > maxhist) {
						maxhist = histarray[p.x][p.y];
						bar = 1;
					}
					if (histarray[p.x][p.y] < minhist) {
						minhist = histarray[p.x][p.y];
						bar = 1;
					}
		 			if (inlevel(histarray[p.x][p.y]))
						BufferHisto(p.x, p.y,-1);
				}
				else
					if (p.y > 0)
						BufferPoint(dpy,trajec,pixtra,Data_GC,&Orbits,
										gc,p.x+xmargin,(traheight-p.y)+ymargin);
		 	}
		 	else {
				if ((x1 > t_min_x) && (y1 > t_min_y)) {
					p.x=(int)(((x1-t_min_x)/t_x_range) * (double)trawidth);
					p.y=(int)(((y1-t_min_y)/t_y_range) * (double)traheight);
				}
			if ((p.x<trawidth) && (p.y<traheight))
				if (histogram) {
					bar = 0;
					histarray[p.x][p.y]++;
					if (histarray[p.x][p.y] > maxhist) {
						maxhist = histarray[p.x][p.y];
						bar = 1;
					}
					if (histarray[p.x][p.y] < minhist) {
						minhist = histarray[p.x][p.y];
						bar = 1;
					}
		 			if (inlevel(histarray[p.x][p.y]))
						BufferHisto(p.x, p.y,-1);
				}
				else
					BufferPoint(dpy,trajec,pixtra,Data_GC,&Orbits,
									gc, p.x+xmargin, (traheight-p.y) + ymargin);
			if (!histogram) hcolor = gc;
			if (orbit_in_basin) {
				if ((x1 > min_x) && (y1 > min_y)) {
					p.x=(int)(((x1-min_x)/x_range) * (double)width);
					p.y=(int)(((y1-min_y)/y_range) * (double)height);
				}
				if ((p.x<width) && (p.y<height)) {
					XDrawPoint(dpy,canvas,Data_GC[hcolor],p.x,height - p.y);
					XDrawPoint(dpy,pixmap,Data_GC[hcolor],p.x,height - p.y);
				}
			}
			if (orbit_in_precrit) {
				if ((x1 > min_x) && (y1 > min_y)) {
					p.x=(int)(((x1-min_x)/x_range) * (double)width);
					p.y=(int)(((y1-min_y)/y_range) * (double)height);
				}
				if ((p.x<width) && (p.y<height)) {
					XDrawPoint(dpy,prejec,Data_GC[hcolor],p.x,height - p.y);
					XDrawPoint(dpy,pixpre,Data_GC[hcolor],p.x,height - p.y);
				}
			}
			if (orbit_in_crit) {
				if ((x1 > c_min_x) && (y1 > c_min_y)) {
					p.x=(int)(((x1-c_min_x)/c_x_range) * (double)criwidth);
					p.y=(int)(((y1-c_min_y)/c_y_range) * (double)criheight);
				}
				if ((p.x<criwidth) && (p.y<criheight)) {
					XDrawPoint(dpy,crijec,Data_GC[hcolor],p.x,criheight-p.y);
					XDrawPoint(dpy,pixcri,Data_GC[hcolor],p.x,criheight-p.y);
				}
			}
		 }
#ifdef USE_3D
	}
#endif
	if (bar && updt) {
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
		    updtbar(trajec,traheight/2,traheight,cornerbar,maxperiod,minperiod);
		}
	}
}

void Draw_Diagonal(w)
Window w;
{
	static double x, mix, miy, xrng, yrng;
	static int i, p, q, wid, hei;

	if (w == trajec) {
		wid = trawidth; hei = traheight;
		mix = t_min_x; miy = t_min_y;
		xrng = t_x_range; yrng = t_y_range;
	}
	else if (w == crijec) {
		wid = criwidth; hei = criheight;
		mix = c_min_x; miy = c_min_y;
		xrng = c_x_range; yrng = c_y_range;
	}
	else {
		wid = width; hei = height;
		mix = min_x; miy = min_y;
		xrng = x_range; yrng = y_range;
	}
	for (i=0; i<wid; i++) {
		x = mix + (((double)i/(double)wid)*xrng);
		p=(int)(((x - mix)/xrng) * (double)wid);
		q=(int)(((x - miy)/yrng) * (double)hei);
		if ((p<wid) && (q<hei))
			XDrawPoint(dpy, w, RubberGC, p, hei - q);
	}
}

void
Draw_Axes(w, on)
Window w;
int on;
{
	static double r, mix, miy, xrng, yrng;
	static int i, m, n, wid, hei;
	static xy_t p;
#ifdef USE_3D
	extern int plot();
	extern double window_distance;
	triple xyz;

	if (orbits_3d && lyap) {
		for (i=0; i<trawidth; i++) {
			xyz.x = t_min_x+(((double)i/(double)trawidth)*t_x_range);
			xyz.y = 0;
			xyz.z = 0;
			if (plot( xyz, &p));
				BufferPoint(dpy,trajec,pixtra,Data_GC,&Orbits,
								on, p.x, (traheight-p.y));
		}
		for (i=0; i<traheight; i++) {
			xyz.y = t_min_y+(((double)i/(double)traheight)*t_y_range);
			xyz.x = 0;
			xyz.z = 0;
			if (plot( xyz, &p));
				BufferPoint(dpy,trajec,pixtra,Data_GC,&Orbits,
								on, p.x, (traheight-p.y));
		}
		for (i=0; i<traheight; i++) {
			xyz.z = ((double)i/(double)traheight) * 2.0 * max_x;
			xyz.x = 0;
			xyz.y = 0;
			if (plot( xyz, &p));
				BufferPoint(dpy,trajec,pixtra,Data_GC,&Orbits,
								on, p.x, (traheight-p.y));
		}
	}
	else {
#endif
		if (w == trajec) {
			wid = trawidth; hei = traheight;
			mix = t_min_x; miy = t_min_y;
			xrng = t_x_range; yrng = t_y_range;
		}
		else if (w == crijec) {
			wid = criwidth; hei = criheight;
			mix = c_min_x; miy = c_min_y;
			xrng = c_x_range; yrng = c_y_range;
		}
		else {
			wid = width; hei = height;
			mix = min_x; miy = min_y;
			xrng = x_range; yrng = y_range;
		}
		for (i=0; i<wid; i++) {
			r = mix + (((double)i/(double)wid)*xrng);
			m=(int)(((r - mix)/xrng) * (double)wid);
			n=(int)(((0.0 - miy)/yrng) * (double)hei);
			if ((m<wid) && (n<hei))
				XDrawPoint(dpy, w, RubberGC, m, hei - n);
		}
		for (i=0; i<hei; i++) {
			r = miy + (((double)i/(double)hei)*yrng);
			m=(int)(((r - miy)/yrng) * (double)hei);
			n=(int)(((0.0 - mix)/xrng) * (double)wid);
			if ((n<wid) && (m<hei))
				XDrawPoint(dpy, w, RubberGC, n, m);
		}
#ifdef USE_3D
	}
#endif
	FlushBuffer(dpy,trajec,pixtra,Data_GC,&Orbits,on,on+1);
}

void
Redraw_Hist()
{
	static int color, i, j, diff;
	static double ncols, power;
	extern int **histarray;
	extern int xmargin, ymargin, minhist, maxhist, start, cornerbar;
	extern void fillbar();

	maxhist = 0; minhist = MAXINT;
	for (i=0; i< traheight; i++)
		for (j=0; j< trawidth; j++)
			if (histarray[j][i]) {
				maxhist = Max(histarray[j][i], maxhist);
				minhist = Min(histarray[j][i], minhist);
			}
	if ((minhist == MAXINT) || (maxhist == 0))
		return;
	else if (maxhist == minhist)
		diff = 1;
	else
		diff = maxhist - minhist;
	for (i = 0 ; i < MAXCOLOR; ++i)
		Histog.npoints[i] = 0;
	ncols = (double)(numcolors - start - 1);
	for (i=0; i< traheight; i++)
		for (j=0; j< trawidth; j++) {
			if (histarray[j][i]) {
				power = (double)(histarray[j][i] - minhist)/(double)diff;
				color = get_hist_index(power);
			}
			else
				color = 0;
			if (Histog.npoints[color] == MAXPOINTS) {
				XDrawPoints(dpy, trajec, Data_GC[color], Histog.data[color],
					Histog.npoints[color], CoordModeOrigin);
				XDrawPoints(dpy, pixtra, Data_GC[color], Histog.data[color],
					Histog.npoints[color], CoordModeOrigin);
				Histog.npoints[color] = 0;
			}
			Histog.data[color][Histog.npoints[color]].x = j + xmargin;
			Histog.data[color][Histog.npoints[color]].y = 
				(traheight - i) + ymargin;
			++Histog.npoints[color];
		}
	FlushBuffer(dpy,trajec,pixtra,Data_GC,&Histog,0,numcolors);
	if (showbar == 1)
		fillbar(hisbar, HISBARHGT, HISBARHGT, cornerbar);
	else if (showbar == 2)
		fillbar(trajec, traheight/2, traheight, cornerbar);
}

void
AllFlushBuffer() {
	FlushBuffer(dpy, which, pixmap, Data_GC, &Points, 0, numcolors);
	FlushBuffer(dpy, trajec, pixtra, Data_GC, &Orbits, 0, numcolors);
	FlushBuffer(dpy, prejec, pixpre, Data_GC, &Basins, 0, numcolors);
	FlushBuffer(dpy, trajec, pixtra, Data_GC, &Histog, 0, numcolors);
}

void
AllInitBuffer() {
	InitBuffer(&Points, MAXCOLOR);
	InitBuffer(&Orbits, MAXCOLOR);
	InitBuffer(&Basins, MAXCOLOR);
	InitBuffer(&Histog, MAXCOLOR);
}
