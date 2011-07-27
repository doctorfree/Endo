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

#include "endo.h"

int maxcrit = 4, numattrs = 0;
int begin, first, middle, second;
double lastx, lasty;
extern int idown;

void
initial_q2(p)
double *p;
{
	if ((p[2] == 0.0) || (p[1] == 0.0))
		return;
/*	To find a critical point on the Y-axis, use this */
/*
 *	start_x = 0.0;
 *	start_y = ((p[0]*p[5]/p[2]) - p[3])/p[1];
 */
/*	To find a critical point on the X-axis, use this */
/*
 *	start_y = 0.0;
 *	start_x = ((p[0]*p[5]) - (p[2]*p[3]))/((2.0*p[2]*p[4])-(p[0]*p[1]));
 */
/*	To find a critical point with x=1, use this */
/*
 *	start_x = 1.0;
 *	start_y = ((p[0]*(p[5]+p[1])/p[2]) - p[3] - (2.0*p[4]))/p[1];
 */
/*	To find a critical point with x=-1, use this */
/*
 *	start_x = -1.0;
 *	start_y = ((p[0]*(p[5]-p[1])/p[2]) - p[3] + (2.0*p[4]))/p[1];
 */
/*	To find a critical point on the diagonal, use this */
 	start_y = (p[1]*p[2])+(2.0*p[2]*p[4])-(p[0]*p[1]);
	if (start_y == 0.0)
		start_x = 0.0;
	else {
 		start_x = ((p[0]*p[5]) - (p[2]*p[3]))/start_y;
		start_y = start_x;
	}
}

void
initial_alex(p)
double *p;
{
	static double t;
	extern double sqrt();

	while (((t = 
	(p[0]*p[1])+1.0-(2.0*(p[0]-p[1])*start_y)-(4.0*start_y*start_y))<0.0) &&
	(start_y > 1.0e-12))
		start_y *= 0.5;
	if (t >= 0.0)
		start_x = 0.5 * sqrt(t);
}

void
initial_secant(p)
double *p;
{
	extern double poly();

	start_x = poly(start_y, p);
}

void
initial_julia(p)
double *p;
{
	extern double sqrt();

	start_x = start_y = 0.0;
	if ((p[3] >= 0.0) || (p[2] == 0.0))
		return;
	start_y = (p[2]/2.0) * sqrt(-p[3]);
}

void
initial_brussel(p)
double *p;
{
	static double a, b;
	extern double sqrt();

	if ((p[0] == 0.0) || ((p[0] + 1.0) == 0.0))
		return;
	a = (p[1] + 1.0) - (1.0 / p[0]);
	b = a / (p[0] + 1.0);
	if (b > 0.0)
		start_x = start_y = ABS(sqrt(b));
}

void
initial_gucken(p)
double *p;
{
	if ((p[2] == 0.0) || ((p[0] + p[1]) == 0.0))
		return;
	start_x = start_y = p[1] / (p[2] * (p[0] + p[1]));
}

/* Pick a point on the critical arc as the initial condition */
void
initial_critical(p)
double *p;
{
	static int i, j, done;
	static pair c;

	done = 0;
	for (i=0; i<250; i++) {
		for (j=0; j<250; j++) {
			if (ABS(((*deriv)((double)i, (double)j, p)).x) < cdelt) {
				start_x = (double)i; start_y = (double)j;
				c = (*map)((double)i, (double)j, p);
				if (ABS(((*deriv)(c.x, c.y, p)).x) < cdelt) {
					done = 1;
					break;
				}
			}
		}
		if (done) break;
	}
}

/* Pick a point on the critical curve where x = y/2 */
void
initial_logistic(p)
double *p;
{
	static double a, b, c, q;
	extern double sqrt();

	if ((p[0] == 0.0) || (p[1] == 0.0))
		return;
	a = 128.0 * p[0] * p[1];
	b = -96.0 * p[0] * p[1];
	c = (15.0 * p[0] * p[1]) + p[0] + p[1] - 1.0;
	if (b < 0)
		q = -0.5 * (b - (sqrt((b*b) - (4.0*a*c))));
	else
		q = -0.5 * (b + (sqrt((b*b) - (4.0*a*c))));
	if ((start_x = Max(q/a, c/q)) > 0.5) {
		if ((start_x = Min(q/a, c/q)) <= 0.0) {
			start_x = 0.3; 
			start_y = 0.6;
		}
		else {
			if (start_x <= 0.5)
				start_y = 2.0 * start_x;
			else {
				start_x = 0.3; 
				start_y = 0.6;
			}
		}
	}
	else {
		if (start_x > 0.0)
			start_y = 2.0 * start_x;
		else {
			start_x = 0.3; 
			start_y = 0.6;
		}
	}
}

int
preiterate(c, i, x1, y1)
int c, i;
double x1, y1;
{
	static int n;
	extern void draw_precrit();

	if (i > maxcrit)
		return(0);
	if (precrit == 1) {
		if (ABS(((*deriv)(x1, y1, params)).x) < cdelt) {
			draw_precrit(x,y,(i%first)+begin);
			return(0);
		}
  	}
	else if (precrit == 2) {/* compute pre-iterates of the diagonal */
		if (ABS(x1 - y1) < cdelt) {
			draw_precrit(x,y,(i%first)+begin);
			return(0);
		}
  	}
  	else if (precrit == 4) {/* compute pre-iterates of the origin */
		if ((ABS(x1) < cdelt) && (ABS(y1) < cdelt)) {
			draw_precrit(x,y,(i%second)+middle);
			return(0);
		}
  	}
  	else if (precrit == 5) {/* compute pre-iterates of the origin */
		n = 1;
		if (c < 2) {/* and diagonal, color origin differently */
			if (ABS(x1 - y1) < cdelt) {
				draw_precrit(x,y,(i%second)+middle);
				n = 2;
			}
		}
		if ((ABS(x1) < cdelt) && (ABS(y1) < cdelt)) {
			draw_precrit(x,y,(i%first)+begin);
			n = 0;
		}
		return(n);
	}
  	else if (precrit == 6) {/* compute pre-iterates of the origin */
		n = 1;
		if (c < 2) {/* and critical, color origin differently */
			if (ABS(((*deriv)(x1, y1, params)).x) < cdelt) {
				draw_precrit(x,y,(i%second)+middle);
				n = 2;
			}
		}
		if ((ABS(x1) < delta) && (ABS(y1) < delta)) {
			draw_precrit(x,y,(i%first)+begin);
			n = 0;
		}
		return(n);
  	}
}

double
set_lyapunov(a, b)
double a, b;
{
	double s;
	extern double sqrt();

	s = (b*b) - (4.0*a);
	if (s < 0.0) /* 4a > b^2 => a > 0 so return the positive average */
		return(a);
	s = sqrt(s);
	return((b + s)/2.0); /* the smaller exponent is (b - s)/2 */
}

compendo()
{
	register i, j;
	static int foundit, crit_hit, index, started=0;
	static int color=MAXCOLOR; 
	static double radius;
	double tr_total, total, tr_prod, prod, alpha, beta, lyapunov; 
	double attrs[MAXATTRS][4];
#ifdef mips
	register double d, x1, y1;
#else
	double d, x1, y1; 
#endif
	pair coordinates;
	int found_crit;
	extern int lowrange, lower, upper;
	extern int xmargin, ymargin, animate, mandel, axes;
#ifdef NorthSouth
	extern double singularity, S_singularity, pB;
	extern void calculate_params();
	extern void solve_quadratic(), gamma_quadratic();
#endif
	extern void save_to_file(),draw_portrait();
	extern void draw_critical();
	extern double drand48(), atan2();

	foundit=0;		/* whether we've detected an attractor */
	found_crit=0;		/* whether we've detected a critical point */
	crit_hit = 1;	/* whether a critical pre-iterate has been found */
	if (!run)
		return TRUE;
	if (lyap) {
		params[p1] = x; params[p2] = y;
		if ((!Xflag) && (!Yflag)) {
#ifdef NorthSouth
			if (mapindex == 7)  /* North-South model */
				calculate_params(mapindex, params);
#endif
			/*double logistic*/
			if ((mapindex == (9+MAP_OFF)) || (mapindex == (10+MAP_OFF)))
				initial_logistic(params);
			if (mapindex == 2) /* Guckenheimer Leslie Matrix */
				initial_gucken(params);
			if (mapindex == (14+MAP_OFF)) /* Brusselator */
				initial_julia(params);
			if (mapindex == (15+MAP_OFF)) /* Brusselator */
				initial_brussel(params);
			if (mapindex == (16+MAP_OFF)) /* Brusselator */
				initial_secant(params);
			if (mapindex == (17+MAP_OFF)) /* Alexander */
				initial_alex(params);
			if (mapindex == (22+MAP_OFF)) /* Mira-Agg (Q2) */
				initial_q2(params);
		}
		if (lyap == 3) {
			if (started) {
				start_x = lastx; start_y = lasty;
			}
			else
				started = 1;
		}
		if (lyap == 4)
			initial_critical(params);
		x1 = start_x; y1 = start_y;
	}
	else {
#ifdef NorthSouth
		if (mapindex == 7)
			if ((x == singularity) || (y == S_singularity))/* N-S model #1*/
				return FALSE;
#endif
		x1 = x; y1 = y;
	}
	if (animate) {
		XCopyArea(dpy,pixtra,trajec,Data_GC[1],xmargin, ymargin,
				trawidth,traheight,xmargin,ymargin);
		XFillRectangle(dpy, pixtra, Data_GC[0], xmargin, ymargin, 
				trawidth, traheight);
		if (axes)
			Draw_Axes(trajec, axes);
	}
	if (mapindex == (7+MAP_OFF)) /* Rotor map */
		if (ABS(params[1]) < 1.0)
			return FALSE;
	/* initialize color bands to begin---first-----middle---second
	 * based on value of negative where begin is the initial color
	 * index, first a range of color indices, middle divides the
	 * color wheel, and second is another range of indices.
	 */
	if (negative) {
		begin = MINCOLINDEX;
		first = numfreecols-1;
		middle = STARTCOLOR;
		second = lowrange-1;
	}
	else {
		middle = MINCOLINDEX;
		second = numfreecols-1;
		begin = STARTCOLOR;
		first = lowrange-1;
	}
	/*
	 * calculate the determinant of the jacobian and color the point
	 * if it's on the critical curve (that is, if the determinant
	 * is "close" to zero).
	 */
	if (critical) {
		d = ABS(((*deriv)(x1, y1, params)).x);
		if (d < cdelt) {
			draw_critical(crijec, x1,y1,begin);
			crit_pts[0][numcrits] = x1;	
			n_crit_pts[0][numcrits] = x1;	
			crit_pts[1][numcrits] = y1;	
			n_crit_pts[1][numcrits++] = y1;	
			found_crit=1;
		}
	}
	/*
	 * initialize periods (how many iterates till within delta of periodic)
	 * and indices (index into the color map based upon value of periods)
	 */
	periods[frame][perind[frame]] = indices[frame][perind[frame]] = 0;
	for (i=0;i<settle;i++) {
		coordinates = (*map)(x1, y1, params);
		x1 = coordinates.x;
		y1 = coordinates.y;
		if ((found_crit) && (critical) && (i < maxcrit)) {
			draw_critical(crijec, x1,y1,(i+1)%first+begin);
			if ((i == 0) && (d = ABS(((*deriv)(x1, y1, params)).x)) < cdelt)
				set_arc(x1, y1);
		}
		if (crit_hit)
			crit_hit = preiterate(crit_hit, i, x1, y1);
		if (mandel) {	/* we test to see if going to infinity */
			radius = (x1*x1) + (y1*y1);
			if (radius > maxradius) {
				if (mandel == 4)
  					indices[frame][perind[frame]] = (i % lowrange) + STARTCOLOR;
				else
  					indices[frame][perind[frame]] = i % 2;
				periods[frame][perind[frame]] = -i;
				if (find)
					basins[frame][perind[frame]] = 0;
  				BufferPoint(dpy,which,pixmap,Data_GC,&Points,
					  indices[frame][perind[frame]++],point.x,height-point.y-1);
				lastx = x1; lasty = y1;
				return FALSE;
			}
		}
	}
	if (portrait)
		if (lyap)
			color=(((((params[p1]-min_x)/x_range)+
				((params[p2]-min_y)/y_range))/2.0)*second) + middle;
		else
			if (++color >= numcolors)
				color = begin;
		prod = 1.0; total = 0.0;
		tr_prod = 1.0; tr_total = 0.0;
		for (i=0;i<dwell;i++) {
			coords[frame][0][i] = x1;
			coords[frame][1][i] = y1;
			coordinates = (*map)(x1, y1, params);
			x1 = coordinates.x;
			y1 = coordinates.y;
			if (mandel) {	/* we test to see if going to infinity */
				radius = (x1*x1) + (y1*y1);
				if (radius > maxradius) {
					if (mandel == 4)
  						indices[frame][perind[frame]] = 
								((i+settle) % lowrange) + STARTCOLOR;
					else
  						indices[frame][perind[frame]]=(i+settle)%2;
					periods[frame][perind[frame]] = -(i+settle);
					if (find)
						basins[frame][perind[frame]] = 0;
  					BufferPoint(dpy,which,pixmap,Data_GC,&Points,
					  indices[frame][perind[frame]++],point.x,height-point.y-1);
					if (portrait) {
						draw_portrait(x1,y1,color);
						FlushBuffer(dpy, trajec, pixtra, Data_GC, &Orbits,
										color, color+1);
					}
					lastx = x1; lasty = y1;
					return FALSE;
				}
			}
		/*
 		* calculate the determinant of the jacobian and color the point
 		* if one of its iterates falls on the critical curve. In this way,
 		* we display the pre-iterates of the critical curve (colored 
 		* according to how many iterates it took to get to the critical
 		* curve). If precrit is set to 2 (-P 4 argument), then we calculate
		* instead the pre-iterates of the diagonal. If precrit is set to
		* 4 (-P 6 argument), then we calculate the pre-images of the origin.
		* If precrit is set to 5 (-P 7 argument), then we calculate both
		* pre-images of the diagonal and origin. If precrit == 6, then
		* the pre-images of the critical curve and the origin are calculated
 		*/
		if (crit_hit)
			preiterate(crit_hit, i+settle, x1, y1);
		if ((found_crit) && (critical) && ((settle + i) < maxcrit)) {
		  draw_critical(crijec, x1,y1,(i+settle+1)%first+begin);
		  if ((settle==0)&&(i==0)&&(d=ABS(((*deriv)(x1,y1,params)).x))<cdelt)
		  	set_arc(x1, y1);
		}
		if ((lyap >= 2) && (mandel < 2)) {
			/* first, sum the determinants */
			if ((d = ABS(((*deriv)(x1, y1, params)).x))>=MAXFLOAT) {
				total += log(prod);
				prod = 1.0;
				total += log(d);
			}
			else {
				prod *= d;
				/* we need to prevent overflow and underflow */
				if ((prod > 1.0e12) || (prod < 1.0e-12)) {
					if (prod == 0.0)
						total += LN_MINDOUBLE;
					else if (prod < 0.0)
						total += LN_MAXDOUBLE;
					else
						total += log(prod);
					prod = 1.0;
				}
			}
			/* next, sum the traces */
			if ((d = ABS(((*deriv)(x1, y1, params)).y))>=MAXFLOAT) {
				tr_total += log(tr_prod);
				tr_prod = 1.0;
				tr_total += log(d);
			}
			else {
				tr_prod *= d;
				/* we need to prevent overflow and underflow */
				if ((tr_prod > 1.0e12) || (tr_prod < 1.0e-12)) {
					if (tr_prod == 0.0)
						tr_total += LN_MINDOUBLE;
					else if (tr_prod < 0.0)
						tr_total += LN_MAXDOUBLE;
					else
						tr_total += log(tr_prod);
					tr_prod = 1.0;
				}
			}
		}
		if (find) {	/* if we are looking for basins of attraction */
		  coordinates = (*map)(x1, y1, params);
		  for (j=0;j<numattrs;j++) {
			if ((ABS(attrs[j][0] - x1) < delta) && 
				(ABS(attrs[j][1] - y1) < delta) &&
				(ABS(attrs[j][2] - coordinates.x) < cdelt) && 
				(ABS(attrs[j][3] - coordinates.y) < cdelt)) {
				if (foundit) {	/* we're close to two attractors */
					foundit = 0;
					break;
				}
				else /* we're within delta of a previous point */
					foundit=j+1;
			}
		  }
		  if (foundit) {
			basins[frame][perind[frame]] = foundit;
			periods[frame][perind[frame]] = i + settle;
		  }
		}
		if (portrait) {
		  if (attractors) {	 /* if we are drawing the attractors */
			if (foundit)
		  		for (j=i;j<dwell;j++) {
					coordinates = (*map)(x1, y1, params);
					x1 = coordinates.x;
					y1 = coordinates.y;
					draw_portrait(x1,y1,color);
		  		}
		  }
		  else {	/* we are drawing orbits */
			if (upper) 
				if (x1 < y1)
					draw_portrait(x1,y1,color);
			if (lower) 
				if (x1 > y1)
					draw_portrait(x1,y1,color);
			if (x1 == y1)
				draw_portrait(x1,y1,color);
		  }
		}
		if (foundit && (!attractors) && (!lyap))
			break;
	}
	if (precrit == 3) { /* diagonal divides attractors */
		if (x1 > y1)
			indices[frame][perind[frame]] = 
					(periods[frame][perind[frame]] % second) + middle;
		else {
			indices[frame][perind[frame]] = 
					(periods[frame][perind[frame]] % first) + begin;
			periods[frame][perind[frame]] *= -1;
		}
	}
	else if (precrit == 7) { /* y-axis divides attractors */
		if (x1 > 0)
			indices[frame][perind[frame]] = 
				(periods[frame][perind[frame]] % second) + middle;
		else {
			indices[frame][perind[frame]] = 
				(periods[frame][perind[frame]] % first) + begin;
			periods[frame][perind[frame]] *= -1;
		}
	}
	else if (precrit == 8) { /* x-axis divides attractors */
		if (y1 > 0)
			indices[frame][perind[frame]] = 
			(periods[frame][perind[frame]] % second) + middle;
		else {
			indices[frame][perind[frame]] = 
			(periods[frame][perind[frame]] % first) + begin;
				periods[frame][perind[frame]] *= -1;
		}
	}
	else if (precrit == 9)				/* color rate of attraction */
		indices[frame][perind[frame]] = /* according to dist from origin */
				((int)(radius*numfreecols/maxradius) % second) + middle;
	else if (precrit == 10) { 			/* color rate of attraction */
		indices[frame][perind[frame]] = /* according to angle with x-axis */
				((int)((atan2(y1,x1)+M_PI)*numfreecols/M_2PI) % second) 
					+ middle - 1;
	}
	else if (precrit == 11) { /* x and y axes divide attractors */
		if (y1 > 0) {
	  		if (x1 > 0)
				indices[frame][perind[frame]] = 
			  			(periods[frame][perind[frame]] % second) + middle;
	  		else 
				indices[frame][perind[frame]] = (second - 
						(periods[frame][perind[frame]]%second)-1)+middle;
		}
		else {
	  		if (x1 > 0)
				indices[frame][perind[frame]] = 
						(periods[frame][perind[frame]] % first) + begin;
	  		else
				indices[frame][perind[frame]] = 
						(first-(periods[frame][perind[frame]]%first)-1) + begin;
		}
	}
	else if (!(find && (!foundit))) {
		if (basins[frame][perind[frame]] && numattrs) {
			j = numfreecols/numattrs;
			indices[frame][perind[frame]]=(periods[frame][perind[frame]]%j) +
					((basins[frame][perind[frame]]-1)*j) + STARTCOLOR;
		}
		else
			indices[frame][perind[frame]] = 
					(periods[frame][perind[frame]] % second) + middle;
	}
	if ((!lyap) && find && (!foundit)) { /* didn't find a known attractor */
		if (!((x1 == y1) && (precrit == 3))) { /* disallow diagonal points */
		  if (numattrs < MAXATTRS) {
			attrs[numattrs][0] = x1;
			attrs[numattrs][1] = y1;
			coordinates = (*map)(x1, y1, params);
			attrs[numattrs][2] = coordinates.x;
			attrs[numattrs][3] = coordinates.y;
			numattrs++;
			printf("Adding pair #%d ((%lf, %lf), (%lf, %lf))\n",
					numattrs,x1,y1,coordinates.x,coordinates.y);
		  }
		  else
		    fprintf(stderr,"Warning: found more than %d attractors\n",MAXATTRS);
		}
	}
	lastx = x1; lasty = y1;
	if ((lyap >= 2) && (mandel < 2)) {
		if (prod == 0.0)
			total += LN_MINDOUBLE;
		else if (prod < 0.0)
			total += LN_MAXDOUBLE;
		else
			total += log(prod);
		if (tr_prod == 0.0)
			tr_total += LN_MINDOUBLE;
		else if (tr_prod < 0.0)
			tr_total += LN_MAXDOUBLE;
		else
			tr_total += log(tr_prod);
		alpha = (total * M_LOG2E) / (double)dwell;
		beta = (tr_total * M_LOG2E) / (double)dwell;
		lyapunov = set_lyapunov(alpha, beta);
			if (lyapunov > 0) {
				index = periods[frame][perind[frame]] = (int)(lyapunov*first);
				indices[frame][perind[frame]]=(index % first)+begin;
			}
			else {
				index = periods[frame][perind[frame]] = (int)(-lyapunov*second);
				indices[frame][perind[frame]] = (index % second) + middle;
			}
		if (lyapunov > maxexp)
			maxexp = lyapunov;
		if (lyapunov < minexp)
			minexp = lyapunov;
  		BufferPoint(dpy,which,pixmap,Data_GC,&Points,
				indices[frame][perind[frame]++],point.x,height-point.y-1);
	}
	else if ((lyap >= 2) && (mandel == 2)) { /* draw Mandelbrot black */
		periods[frame][perind[frame]]=indices[frame][perind[frame]] = 0;
  		BufferPoint(dpy,which,pixmap,Data_GC,&Points,
				indices[frame][perind[frame]++],point.x,height-point.y-1);
	}
	else if ((lyap >= 2) && (mandel == 3)) { /* display periods */
		if (foundit)
			periods[frame][perind[frame]] = settle + i;
		else
	   		periods[frame][perind[frame]] = 0;
		indices[frame][perind[frame]] = 
			(periods[frame][perind[frame]]%second)+middle;
  		BufferPoint(dpy,which,pixmap,Data_GC,&Points,
				indices[frame][perind[frame]++],point.x,height-point.y-1);
	}
	else if (!lyap)
  		   BufferPoint(dpy,which,pixmap,Data_GC,&Points,
				indices[frame][perind[frame]++],point.x,height-point.y-1);
	else
		perind[frame]++;
	if (portrait)
		FlushBuffer(dpy, trajec, pixtra, Data_GC, &Orbits, color, color+1);
	lastx = x1; lasty = y1;
	return FALSE;
}

set_arc(x1, y1)
double x1, y1;
{
	extern void draw_critical();

	draw_critical(crijec, x1,y1,1);
	draw_critical(crijec, x,y,1);
	A0.x = x; A0.y = y;
	A1.x = x1; A1.y = y1;
	if (!found_arc) {
		B0.x = x; B0.y = y;
		B1.x = x1; B1.y = y1;
		found_arc = 1;
		if (find_arcs(B0, B1) == TRUE) {
			found_arc = 1;
			if (!idown)
				Show_Info();
		}
		else
			found_arc = 0;
	}
	else {
		if ((B0.x == A0.x) && (B0.y == A0.y)) {
			if (((x - B1.x)*(x - B1.x) + (y - B1.y)*(y - B1.y)) >
			   ((B0.x-B1.x)*(B0.x-B1.x) + (B0.y-B1.y)*(B0.y-B1.y))) {
				B0.x = x; B0.y = y;
				B1.x = x1; B1.y = y1;
				if (find_arcs(B0, B1) == TRUE) {
					found_arc = 1;
					if (!idown)
						Show_Info();
				}
				return;
			}
		}
		if (find_arcs(A0, A1) == TRUE) {
			found_arc++;
			if (!idown)
				Show_Info();
		}
	}
}

find_arcs(p0, p1)
pair p0, p1;
{
	static int i;
	static double mix=0.0, max=0.0, miy=0.0, may=0.0;
	extern int found_arc;
	extern void draw_critical();

	if (!found_arc) {
		fprintf(stderr,"No intersections detected\n");
		return FALSE;
	}
	if (p0.x < p1.x) {
		mix = p0.x; max = p1.x;
	}
	else if (p0.x > p1.x) {
		mix = p1.x; max = p0.x;
	}
	if (p0.y < p1.y) {
		miy = p0.y; may = p1.y;
	}
	else if (p0.y > p1.y) {
		miy = p1.y; may = p0.y;
	}
	if ((p0.x == p1.x) && (p0.y == p1.y)) {
		fprintf(stderr,"Found a fixed point at (%lf, %lf)\n",p0.x, p0.y);
		return FALSE;
	}
	numarcs = 0;
	for (i=0;i<numcrits;i++) {
		if ((crit_pts[0][i] >= mix) && (crit_pts[0][i] <= max) &&
			(crit_pts[1][i] >= miy) && (crit_pts[1][i] <= may)) {
			draw_critical(crijec, crit_pts[0][i],crit_pts[1][i],1);
			n_crit_arc[0][numarcs] = crit_arc[0][numarcs] = crit_pts[0][i];
			n_crit_arc[1][numarcs] = crit_arc[1][numarcs++] = crit_pts[1][i];
		}
	}
	return TRUE;
}

