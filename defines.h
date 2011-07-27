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

#ifndef DEFINES_H
#define ABS(a)	(((a)<0) ? (0-(a)) : (a) )
#define Min(x,y) ((x < y)?x:y)
#define Max(x,y) ((x > y)?x:y)
#define MAXPARAMS 24
#define MAXDEMOS 10
#define HALF 12
#define STRIPE 7
#define CG 1
#define WINDOW_DISTANCE 10.0
#define S_HEIGHT (XDisplayHeight(dpy, screen) - 40)
#define S_WIDTH XDisplayWidth(dpy, screen)
#define INFO_WIDTH 130
#define INFO_HEIGHT 27
#define HELP_WIDTH 70
#define HELP_HEIGHT 50
#define TITS_WIDTH 200
#define TITS_HEIGHT 100

#define TITLE_NAME "Introducing"
#define TRAJ_NAME "Point Trajectories"
#define ATTR_NAME "Basins of Attraction"
#define CRIT_NAME "Critical Curves"
#define PRE_NAME "Pre-Critical Curves"
#define INFO_NAME "Parameter Values"
#define HELP_NAME "Help"
#define HIST_NAME "Histogram of Point Trajectories"
#define BIF_NAME "Bifurcation Diagrams"
#define LYAP_NAME "Lyapunov Exponents"
#define BAR_NAME "Color Index"
#define ADJ_NAME "Index Map"
#define SELECT_NAME \
      "Select  Highlighted  Setup  with  Right  Mouse  Button  or  Space  Bar"
#define MAXFRAMES 1
#ifndef MAXPOINTS
#define MAXPOINTS  128
#endif
#ifndef MAXCOLOR
#define MAXCOLOR  256
#endif
#define MAXATTRS  1024
#define DEMO_WIDTH 512
#define DEMO_HEIGHT 512
#define DEMO_DWELL 128
#define DEMO_SETTLE 128
#define DEMO_16_WIDTH 256
#define DEMO_16_HEIGHT 256
#define HISBARWID 50
#define HISBARHGT 480
#define ADJWIDTH 256
#define ADJHEIGHT 256
#define MAXDIVS 12
#ifdef NorthSouth
#ifdef Gardini
#ifdef Plendo
#define NUMMAPS 29	/* remember to change NUMDEFS as well */
#define NUMDEFS 58	/* always twice NUMMAPS */
#define MAP_OFF 4	/* Map index offset */
#else
#define NUMMAPS 23	/* remember to change NUMDEFS as well */
#define NUMDEFS 46	/* always twice NUMMAPS */
#define MAP_OFF 4	/* Map index offset */
#endif
#else
#ifdef Plendo
#define NUMMAPS 26	/* remember to change NUMDEFS as well */
#define NUMDEFS 52	/* always twice NUMMAPS */
#define MAP_OFF 1	/* Map index offset */
#else
#define NUMMAPS 22	/* remember to change NUMDEFS as well */
#define NUMDEFS 44	/* always twice NUMMAPS */
#define MAP_OFF 1	/* Map index offset */
#endif
#endif
#else
#ifdef Gardini
#ifdef Plendo
#define NUMMAPS 28	/* remember to change NUMDEFS as well */
#define NUMDEFS 56	/* always twice NUMMAPS */
#define MAP_OFF 3	/* Map index offset */
#else
#define NUMMAPS 24	/* remember to change NUMDEFS as well */
#define NUMDEFS 48	/* always twice NUMMAPS */
#define MAP_OFF 3	/* Map index offset */
#endif
#else
#ifdef Plendo
#define NUMMAPS 25	/* remember to change NUMDEFS as well */
#define NUMDEFS 50	/* always twice NUMMAPS */
#define MAP_OFF 0	/* Map index offset */
#else
#define NUMMAPS 21	/* remember to change NUMDEFS as well */
#define NUMDEFS 42	/* always twice NUMMAPS */
#define MAP_OFF 0	/* Map index offset */
#endif
#endif
#endif
#ifndef NULL
#define NULL 0
#endif
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_2PI
#define M_2PI 6.28318530717958647692
#endif
#ifndef M_PI_2
#define M_PI_2 1.5707963267948965580
#endif
#ifndef M_LOG2E
#define M_LOG2E	1.4426950408889634074
#endif

typedef struct {
	int x, y;
} xy_t;

typedef struct {
	int start_x, start_y;
	int last_x, last_y;
	} rubber_band_data_t;

typedef struct {
	double x;
	double y;
} pair;

typedef struct {
	double x, y, z;
} triple;

typedef pair (*PFP)();

#define DEFINES_H
#endif
