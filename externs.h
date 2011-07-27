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

#ifndef EXTERNS_H
extern PFP map; 
extern PFP deriv;
extern pair dnumeric();
extern PFP Maps[];
extern PFP Derivs[];
extern char *Mapnames[];
extern Window stacktop,which,canvas,trajec,crijec,info,help,lyajec,prejec;
extern Window hisbar, colwin;
extern Display*	dpy;
extern GC Data_GC[], RubberGC, Reverse_GC;
extern pair A0, A1, B0, B1;
extern double	min_x, min_y, x_range, y_range;
extern double max_x, max_y, minexp, maxexp;
extern double x_inc, y_inc, x, y, c_min_x, c_min_y, c_x_range, c_y_range;
extern double t_min_x, t_min_y, t_x_range, t_y_range;
extern int	dwell,settle,width,height,criwidth,criheight,trawidth,traheight;
extern int	numcolors, numfreecols, displayplanes, run;
extern xy_t	point;
extern points_t Points, Orbits, Basins, Histog;
extern Pixmap pixmap, pixtra, pixcri, pixpre;
extern Colormap cmap;
extern XColor	Colors[];
extern int*basins[];
extern int	 *periods[];
extern int	 *indices[];
extern int	screen;
extern double	*coords[][2];
extern double	*crit_pts[];
extern double	*crit_arc[];
extern double	*n_crit_pts[];
extern double	*n_crit_arc[];
extern double a_minimums[], b_minimums[];
extern double a_maximums[], b_maximums[];
extern int	minperiod, maxperiod, p1, p2, histogram, precrit;
extern int	perind[], resized[];
extern int	numwheels, portrait, attractors, find, critical, lyap;
extern int	maxradius, nostart, numcrits, numarcs, randinit, clearflag;
extern int	save, show, spinlength, savefile, showbar, adjcol;
extern int	maxframe, frame, mapindex, orbit, orbits_3d, row, numrows;
extern int STARTCOLOR, MINCOLINDEX, Xflag, Yflag;
extern char	*outname;
extern double params[];
extern double cdelt, delta, start_x, start_y;
extern unsigned long background, foreground;
extern image_data_t rubber_data;
extern void	 resize();
extern void	 redisplay();
extern void	 Cleanup();
extern void	 Spin();
extern void	 show_defaults();
extern void	 StartRubberBand();
extern void	 TrackRubberBand();
extern void	 EndRubberBand();
extern void	 CreateXorGC();
extern void	 InitBuffer();
extern void	 BufferPoint();
extern void	 FlushBuffer();
extern void  AllInitBuffer();
extern void  AllFlushBuffer();
#define EXTERNS_H
#endif
