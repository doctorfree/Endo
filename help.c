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

#define x_str 10

void
print_help() 
{
	static char str[80];
	static int y_str, spacing;
	static int ascent, descent, dir;
	static XCharStruct overall;
	static GC gc;
	extern int numattrs, usedefault;

	if (displayplanes > 1)
		gc = Data_GC[CG];
	else
		gc = Data_GC[1];
	XClearWindow(dpy, help);
	if (!usedefault)
	    init_color(dpy, help, cmap, Colors, STARTCOLOR, MINCOLINDEX, numcolors,
				numwheels, "endo", "Endo", numattrs);
	y_str = 20;
	sprintf(str,"During run-time, interactive control can be exerted via : ");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	XQueryTextExtents(dpy,(XID)XGContextFromGC(gc),"Hey!",
			4,&dir,&ascent,&descent,&overall);
	spacing = ascent + descent + 5;
	y_str += spacing;
	sprintf(str,"	Left mouse button rubber-bands a zoom box");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	Middle mouse button toggles trajectory display ");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	Right mouse button displays a particular trajectory");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	< halves the 'dwell', > doubles the 'dwell'");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	[ halves the 'settle', ] doubles the 'settle'");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	{ halves the 'delta', } doubles the 'delta'");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	- halves the 'cdelt', + doubles the 'cdelt'");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	a toggles display of attractors rather than orbits");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	c clears the orbit/attractor display window");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	C toggles the critical curve window display");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	D flushes the drawing buffer");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	e or E recalculates color indices");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	f toggles looking for periodicity");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	F saves periods to a file");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	G redraws the critical curves");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	g iterates the selected critical arc");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	h or H or ? displays this message");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	i or I toggles display of the information window");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	M decrements, m increments the map index used");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	P toggles the trajectory display");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	p toggles the basins of attraction display");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	r redraws without recalculating");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	R redraws, recalculating with new dwell and settle");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	s or S spins the colorwheel");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	t switches to alternate critical arc");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	u pops back up to the last zoom");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	U pops back up to the first picture");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	v or V displays the values of various settings");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	w decrements, W increments the color wheel index");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	x clears the basins of attraction window");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	z clears the critical curve window");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += spacing;
	sprintf(str,"	q or Q exits");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
	y_str += 2*spacing;
	sprintf(str,"Press 'h', 'H', or '?' to unmap the help window");
	XDrawImageString(dpy,help,gc,x_str,y_str,str,strlen(str));
}
