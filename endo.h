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

/*
 *	@(#) endo.h 11.1 93/11/16 SCOINC
 */
/*
 *	Written by Ron Record (rr@ronrecord.com) 17 Jan 1992.
 */
#ifndef ENDO_H
#include <assert.h>
#ifndef NeXT
#include <values.h>
#endif
#include <math.h>
#include <memory.h>
#include <stdio.h>
#include "x.h"

#ifndef LN_MINDOUBLE
#define LN_MINDOUBLE (M_LN2 * (DMINEXP - 1))
#endif
#ifndef LN_MAXDOUBLE
#define LN_MAXDOUBLE (M_LN2 * DMAXEXP)
#endif

int		screen;
Display*	dpy;

Window stacktop, which, canvas, trajec, crijec, info, help, lyajec, prejec;
Window hisbar, colwin;

pair gaertner(), goodwin(), gucken(), circle(), hump(), circle2(), standard();
#ifdef NorthSouth
pair north(); 
#endif
#ifdef Gardini
pair gard(), gard2(), gard3();
#endif
#ifdef Plendo
pair q1(), q2(), q3(), c1();
#endif
pair rotor(), twistandflip(), dorband(), logistic(), alexander(), lorenz();
pair mira(), peitgen(), julia(), brussel(), secant(), golub(), marty(), baru();
pair A0, A1, B0, B1;
pair dgaertner(), dgoodwin(), dgucken(), dcircle(), dhump(), dcircle2(); 
#ifdef NorthSouth
pair dnorth();
#endif
#ifdef Gardini
pair dgard(), dgard2(), dgard3();
#endif
#ifdef Plendo
pair dq1(), dq2(), dq3(), dc1();
#endif
pair dstandard(), drotor(), dtwistandflip(), dbaru();
pair ddorband(), dlogistic(), dlorenz(), dmira(), dpeitgen(), djulia();
pair dnumeric(), dbrussel(), dsecant(), dgolub(), dmarty(), dalexander();
PFP map; 
PFP deriv;
PFP Maps[NUMMAPS]={ gaertner, goodwin, gucken, circle, hump, circle2, standard,
#ifdef NorthSouth
#ifdef Gardini
	north, gard, gard2, gard3,
#else
	north,
#endif
#else
#ifdef Gardini
	gard, gard2, gard3,
#endif
#endif
	rotor, twistandflip, dorband, logistic, lorenz,
	mira, peitgen, julia, brussel, secant, alexander, golub, marty, baru
#ifdef Plendo
	, q1, q2, q3, c1 
#endif
};
PFP Derivs[NUMMAPS]={ dgaertner, dgoodwin, dgucken, dcircle, dhump, dcircle2, 
	dstandard, 
#ifdef NorthSouth
#ifdef Gardini
	dnorth, dgard, dgard2, dgard3,
#else
	dnorth,
#endif
#else
#ifdef Gardini
	dgard, dgard2, dgard3,
#endif
#endif
	drotor, dtwistandflip, ddorband, dlogistic, dlorenz, dmira, dpeitgen, 
	djulia, dbrussel, dsecant, dalexander, dgolub, dmarty, dbaru
#ifdef Plendo
	, dq1, dq2, dq3, dc1 
#endif
};
char *Mapnames[NUMMAPS]={ "Gaertner", "Goodwin", "Leslie Matrix", "Circle",
		"Hump", "Circle2", "Standard", 
#ifdef NorthSouth
		"Chichilnisky North-South", 
#endif
#ifdef Gardini
		"Gardini figure 8", "Gardini figure 18", "Gardini figure 20",
#endif
		"Rotor", "Twist and Flip", "Dorband Double Logistic", "Double Logistic",
		"Lorenz", "Mira 1973", "Volterra-Lotka", "Julia-Mandelbrot",
		"Brusselator", "Secant Method", "Alexander", 
		"Field and Golubitsky Quilts","Field and Golubitsky Icons","Barugola" 
#ifdef Plendo
	, "Mira Quad (Q1)", "Mira Agg (Q2)", "Mira-Narayaninsamy (Q3)", 
	"Kawakami-Kawasaki (C1)" 
#endif
		};

int color_offset = 96;
int width=490, height=390;
int settle=256, dwell=256;
int criwidth=490, criheight=390;
int trawidth=512, traheight=512;

double	min_x=0.0, min_y=0.0, x_range=1.0, y_range=1.0;
double  max_x=1.0, max_y=1.0;
double  x_inc, y_inc, x, y, c_min_x, c_min_y, c_x_range, c_y_range;
double  t_min_x, t_min_y, t_x_range, t_y_range;
int		run=1, negative=0, numcolors=16, numfreecols, displayplanes;
xy_t	point;
points_t Points, Orbits, Basins, Histog;
Pixmap  pixmap, pixtra, pixcri, pixpre;
Colormap cmap;
XColor	Colors[MAXCOLOR];
int		*basins[MAXFRAMES];
int		*periods[MAXFRAMES];
int		*indices[MAXFRAMES];
double	*coords[MAXFRAMES][2];
double	*crit_pts[2];
double	*crit_arc[2];
double	*n_crit_pts[2];
double	*n_crit_arc[2];
double  a_minimums[MAXFRAMES], b_minimums[MAXFRAMES];
double  a_maximums[MAXFRAMES], b_maximums[MAXFRAMES];
int		minperiod, maxperiod, p1=0, p2=1, precrit=0;
int		perind[MAXFRAMES]={0}, resized[MAXFRAMES]={0}; 
int		numwheels=MAXWHEELS, portrait=1, attractors=0, find=0, critical=0; 
int		maxradius=100, nostart=1, numcrits=0, numarcs=0, randinit=0, lyap=2;
int		save=1, show=0, spinlength=512, savefile=0, clearflag=0;
int		maxframe=0, frame=0, mapindex=-1, orbit=0, histogram=0;
int		found_arc=0, orbits_3d=0, row=1, numrows=1, showbar=0, adjcol=0;
int		STARTCOLOR=17, MINCOLINDEX=33, Xflag=0, Yflag=0;
char	*outname="endo.out";
double  params[MAXPARAMS];
double cdelt=0.05, delta=1.0e-3, start_x=0.3, start_y=0.7, maxexp=0, minexp=0;
unsigned long background, foreground;

GC Data_GC[MAXCOLOR], RubberGC, Reverse_GC;

void	resize();
void	redisplay();
void	Spin();
void	show_defaults();
void	StartRubberBand();
void	TrackRubberBand();
void	EndRubberBand();
void	CreateXorGC();
void	AllInitBuffer();
void	AllFlushBuffer();
#define ENDO_H
#endif
