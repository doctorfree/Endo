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

short delay = 0;

static int numselect, map_width, num_width;
static int ascent, descent, spacing, dir;
static XCharStruct overall;
static char str[256];
static char *Mapdefs[NUMDEFS] = {
"   x(n+1) = A*x(n)*[E - F*x(n)] + B*y(n)*[G - I*y(n)]                        ",
"   y(n+1) = C*y(n)*[J - K*y(n)] + D*x(n)*[L - M*x(n)]                        ",
"   x(n+1) = x(n) + B*x(n)/[1 - y(n+1)] - BC*x(n) - D*x(n)                    ",
"   y(n+1) = y(n)/[A + x(n)] - D*y(n) - E*y(n)                                ",
"   x(n+1) = [A*x(n) + B*y(n)] * exp(-C * [x(n) + y(n)])                      ",
"   y(n+1) = D*x(n)                                                           ",
"   x(n+1) = [E + J*x(n)] - A*sin(2*PI*x(n)) + [E + K*y(n)] - B*sin(2*PI*y(n))",
"   y(n+1) = [F + L*y(n)] - C*sin(2*PI*y(n)) + [I + M*x(n)] - D*sin(2*PI*(n)) ",
"   x(n+1) = A * sin(PI * x(n))] + [B * sin(PI * y(n))]                       ",
"   y(n+1) = C * sin(PI * y(n))] + [D * sin(PI * x(n))]                       ",
"   x(n+1) = [E + J*x(n)] - A*sin(2*PI*x(n)) + [F + K*y(n)] - B*cos(2*PI*y(n))",
"   y(n+1) = [G + L*y(n)] - C*sin(2*PI*y(n)) + [I + M*x(n)] - D*cos(2*PI*(n)) ",
"   x(n+1) = x(n) + A*sin(y(n))                                               ",
"   y(n+1) = y(n) + x(n) + B*sin(y(n))                                        ",
#ifdef NorthSouth
"   x(n+1) = (N_B * N_GNP) + ((1 - N_A) * x(n)) (with I = GNP*(1-gamma))      ",
"   y(n+1) = (B * S_GNP) + ((1 - A) * y(n))     (with I = GNP*(1-gamma))      ",
#endif
#ifdef Gardini
"   x(n+1) = y - Bxy                                                          ",
"   y(n+1) = y - Ax + Cx^3                                                    ",
"   x(n+1) = Bx + y                                                           ",
"   y(n+1) = A + x^2                                                          ",
"   x(n+1) = By                                                               ",
"   y(n+1) = y - Ax + x^2                                                     ",
#endif
"   y(n+1) = (y(n) - rho*sin(x(n)))/C                                         ",
"   x(n+1) = x(n) - y(n+1)                                                    ",
"   x(n+1) = -((x-a)*cos(pi*r/w) - y*sin(pi*r/w) + a)                         ",
"   y(n+1) = -((x-a)*sin(pi*r/w) + y*cos(pi*r/w))                             ",
"   x(n+1) = 4*A*y(n)*[1 - y(n)] + (1 - A)*x(n)                               ",
"   y(n+1) = 4*B*x(n)*[1 - x(n)] + (1 - B)*y(n)                               ",
"   x(n+1) = 4*A*x(n)*[1 - x(n)] + (1 - A)*y(n)                               ",
"   y(n+1) = 4*B*y(n)*[1 - y(n)] + (1 - B)*x(n)                               ",
"   x(n+1) = [1 + A*B]*x(n) - A*x(n)*y(n)                                     ",
"   y(n+1) = [1 - A]*y(n) + A*x(n)*x(n)                                       ",
"   x(n+1) = y(n) + F(x(n)) + B*y(n)*[1 + C*y(n)*y(n)]                        ",
"   y(n+1) = -x(n) + F(x(n+1))x(n)*x(n)                                       ",
" x(n+1) = x+A/2(f(x,y)+f(p(x,y),q(x,y)))  f(x,y)=Dx-Exy  g(x,y)=Gxy-Fy       ",
" y(n+1) = y+A/2(g(x,y)+g(p(x,y),q(x,y)))  p(x,y)=x+Bf(x,y)  q(x,y)=y+Cg(x,y) ",
" x(n+1) = x*x - y*y + A + C*x                                                ",
" y(n+1) = 2*x*y + B + D*C*y                                                  ",
" x(n+1) = x + A(C - B*x + x*x*y - x)                                         ",
" y(n+1) = y + A(B*x - x*x*y)                                                 ",
" x(n+1) = x - [f(x) * (x - y)] / [f(x) - f(y)]                               ",
" y(n+1) = x   where f(x) = Lx^A + Kx^(A-1) + ... + Dx^2 + Cx + B             ",
" x(n+1) = x^2 - y^2 - x - Ay                                                 ",
" y(n+1) = 2xy - Bx + y                                                       ",
" x(n+1) = Gx+F+Asin(2PIx)+Bsin(2PIx)cos(2PIy)+Csin(4PIx)+Dsin(6PIx)cos(4PIy)-Esin(2PIy)",
" y(n+1) = gy+f+asin(2PIy)+bsin(2PIy)cos(2PIx)+csin(4PIy)+dsin(6PIy)cos(4PIx)-esin(2PIx)",
" z(n+1) = (A+B*z*zbar+C*Re(z^F)+G*Re((z/|z|)^F*I)*|z|+E*i)*z+D*(zbar^(F-1))  ",
" where z is a complex number, zbar its conjugate and Re denotes the real part",
" x(n+1) = x*exp(A*(1-B*x) - (N * y))                                       ",
" y(n+1) = x*(1-exp(-N*y)) where N=(D*A)/(1-exp(C*A))                        "
#ifdef Plendo
," x+ = Ax + Cy + D + Ex^2 + Fy^2 + G                                         ",
" y+ = Ix + Jy + B + Kx^2 + Ly^2 + K                                          ",
" x+ = Ax + Cy                                                                ",
" y+ = Dx + Ex^2 + Bxy + Fy                                                   ",
" x+ = x^2 - y^2 + A + Bx                                                     ",
" y+ = 2xy - 5/2(By)                                                          ",
" x+ = Bx + Cy                                                                ",
" y+ = Ax + Dy + Ex^3                                                         "
#endif
};

Show_Info()
{

    extern int found_arc, mandel, maxcrit, numattrs, maxhist, minhist;
    extern double sea_level, sky_level;
#ifdef NorthSouth
    extern double singularity, A_zero, A_one, A_two, A_star, B_zero, B_one;
    extern double S_singularity, H_one, H_zero, O_B_zero, O_B_one;
    extern double S_A_zero, S_A_one, S_A_two, S_A_star, S_B_zero, S_B_one;
    extern double O_S_B_zero, O_S_B_one;
    extern double pB, A2T, A1T, CT, ET, FT, GT;
    extern pair GNP, RK;
    static char *mops[HALF] = { "delta", "s    ", "a1   ", "a2   ", "c1   ",
	      "c2   ", "D    ", "Kbar ", "Lbar ", "alpha", "beta ", "gamma" };
#endif
    static int numparams[NUMMAPS]= {12,5,4,12,4,12,2,
#ifdef NorthSouth
#ifdef Gardini
	12,3,2,2,
#else
	12,
#endif
#else
#ifdef Gardini
	3,2,2,
#endif
#endif
	2,3,2,2,2,3,7,4,3,12,2,7,8,4
#ifdef Plendo
	, 12, 6, 2, 5
#endif
	};
    static int i, x_str, y_str;
    static GC gc;
    static char ops[HALF] = { 'A','B','C','D','E','F','G','I','J','K','L','M' };

    if (displayplanes > 1)
	gc = Data_GC[CG];
    else
	gc = Data_GC[1];
    XClearWindow(dpy, info);
    y_str = 20; x_str = 175;
    sprintf(str,"Iterating the %s Map ",Mapnames[mapindex]);
    XDrawImageString(dpy,info,gc,x_str,y_str,str,strlen(str));
    XQueryTextExtents(dpy,(XID)XGContextFromGC(gc),"X",
			1,&dir,&ascent,&descent,&overall);
    spacing = ascent + descent + 7;
    y_str += spacing; 
    x_str = 40;
    XDrawImageString(dpy,info,gc,x_str,y_str,Mapdefs[2*mapindex],
		strlen(Mapdefs[2*mapindex]));
    y_str += spacing; 
    XDrawImageString(dpy,info,gc,x_str,y_str,Mapdefs[2*mapindex+1],
		strlen(Mapdefs[2*mapindex+1]));
    x_str = 0;
    y_str += 2*spacing; 
#ifdef NorthSouth
    if (mapindex == 7) {
    	sprintf(str,"          South      North");
    	XDrawImageString(dpy,info,gc,x_str,y_str, str, strlen(str));
    	y_str += spacing; 
    	for (i=0;i<HALF;i++) {
			sprintf(str," %s  %8.4f  %8.4f", mops[i],params[i],params[HALF+i]);
			XDrawImageString(dpy,info,gc,x_str,y_str, str, strlen(str));
			y_str += spacing; 
    	}
    }
    else
#endif /* NorthSouth */
    if (mapindex == 24) {
    	sprintf(str,"       X             Y");
    	XDrawImageString(dpy,info,gc,x_str,y_str, str, strlen(str));
    	y_str += spacing; 
    	for (i=0;i<numparams[mapindex];i++) {
			sprintf(str," %c = %8.4f  %c = %8.4f", 
				ops[i], params[i], tolower(ops[i]), params[HALF+i]);
			XDrawImageString(dpy,info,gc,x_str,y_str, str, strlen(str));
        	y_str += spacing; 
		}
	}
	else
		for (i=0;i<numparams[mapindex];i++) {
			sprintf(str,"   %c=%lf",ops[i],params[i]);
			XDrawImageString(dpy,info,gc,x_str,y_str, str, strlen(str));
			y_str += spacing;
		}
    x_str = 50 + (23*overall.width);
    y_str = 5*spacing; 
    sprintf(str,"width = %d, height = %d, run = %d", width,height,run);
    if ((criwidth != width) || (criheight != height))
        sprintf(str,"%s, cwidth=%d, cheight=%d",str,criwidth,criheight);
    XDrawImageString(dpy,info,gc,x_str,y_str,str,strlen(str));
    y_str += spacing;
    sprintf(str,"settle = %d, dwell = %d, maxcrit = %d",settle,dwell,maxcrit);
    XDrawImageString(dpy,info,gc,x_str,y_str,str,strlen(str));
    y_str += spacing;
    sprintf(str,"delta = %6.5f, cdelta = %6.5f, radius = %d",
				delta, cdelt, maxradius);
    XDrawImageString(dpy,info,gc,x_str,y_str,str,strlen(str));
    y_str += spacing;
    if (lyap) {
		if (p1 < HALF) {
#ifdef NorthSouth
		if (mapindex == 7)
            sprintf(str,"x-axis = (%lf,%lf), parameter = South %s, range = %lf",
				min_x, max_x, mops[p1], x_range);
		else
#endif
            sprintf(str,"x-axis = (%lf,%lf), parameter = %c, range = %lf",
			min_x, max_x, ops[p1], x_range);
		}
		else {
#ifdef NorthSouth
		if (mapindex == 7)
            sprintf(str,"x-axis = (%lf,%lf), parameter = North %s, range = %lf",
			min_x, max_x, mops[p1-HALF], x_range);
		else
#endif
            sprintf(str,"x-axis = (%lf,%lf), parameter = %c, range = %lf",
			min_x, max_x, tolower(ops[p1-HALF]), x_range);
		}
    }
    else
        sprintf(str,"x-axis = (%lf,%lf), x-range = %lf",min_x,max_x,x_range);
    XDrawImageString(dpy,info,gc,x_str,y_str,str,strlen(str));
    y_str += spacing;
    if (lyap) {
		if (p2 < HALF) {
#ifdef NorthSouth
		if (mapindex == 7)
            sprintf(str,"y-axis = (%lf,%lf), parameter = South %s, range = %lf",
			min_y, max_y, mops[p2], y_range);
		else
#endif
            sprintf(str,"y-axis = (%lf,%lf), parameter = %c, range = %lf",
			min_y, max_y,ops[p2], y_range);
		}
		else {
#ifdef NorthSouth
		if (mapindex == 7)
            sprintf(str,"y-axis = (%lf,%lf), parameter = North %s, range = %lf",
			min_y, max_y, mops[p2-HALF], y_range);
		else
#endif
            sprintf(str,"y-axis = (%lf,%lf), parameter = %c, range = %lf",
			min_y, max_y, tolower(ops[p2-HALF]), y_range);
		}
    }
    else
        sprintf(str,"y-axis = (%lf,%lf), y-range = %lf",min_y,max_y,y_range);
    XDrawImageString(dpy,info,gc,x_str,y_str,str,strlen(str));
    y_str += spacing;
    if ((c_min_x != min_x) || (c_min_y != min_y) ||
		(c_x_range != x_range) || (c_y_range != y_range)) {
        sprintf(str,"critical axes = (%lf, %lf) x (%lf, %lf)",
		c_min_x, c_min_x + c_x_range, c_min_y, c_min_y + c_y_range);
        XDrawImageString(dpy,info,gc,x_str,y_str,str,strlen(str));
        y_str += spacing;
    }
    if ((t_min_x != min_x) || (t_min_y != min_y) ||
		(t_x_range != x_range) || (t_y_range != y_range)) {
        sprintf(str,"orbit axes = (%lf, %lf) x (%lf, %lf)",
		t_min_x, t_min_x + t_x_range, t_min_y, t_min_y + t_y_range);
        XDrawImageString(dpy,info,gc,x_str,y_str,str,strlen(str));
        y_str += spacing;
    }
    sprintf(str,
	   "numcolors=%d STARTCOLOR=%d MINCOLINDEX=%d histogram=%d sea=%lf sky=%lf",
				numcolors,STARTCOLOR,MINCOLINDEX,histogram,sea_level,sky_level);
    XDrawImageString(dpy,info,gc,x_str,y_str,str,strlen(str));
#ifdef NorthSouth
    if (mapindex == 7) {
        y_str += spacing;
        sprintf(str, "North: A0 =%8.6f A1 =%8.6f A2 =%8.6f A* =%8.6f", 
					A_zero, A_one, A_two, A_star);
        XDrawImageString(dpy,info,gc,x_str,y_str,str,strlen(str));
        y_str += spacing;
        sprintf(str,
			"       H0 =%8.6f H1 =%8.6f B0 =%8.6f (%8.6f) B1 =%8.6f (%8.6f)", 
				H_zero, H_one, B_zero, O_B_zero, B_one, O_B_one);
        XDrawImageString(dpy,info,gc,x_str,y_str,str,strlen(str));
        y_str += spacing;
        sprintf(str,
   				"South: A0 =%8.6f A1 =%8.6f A2 =%8.6f A* =%12.10lf", 
						S_A_zero, S_A_one, S_A_two, S_A_star);
        XDrawImageString(dpy,info,gc,x_str,y_str,str,strlen(str));
        y_str += spacing;
        sprintf(str,
   				"       B0 =%8.6f (%8.6f) B1 =%8.6f (%8.6f)", 
						S_B_zero, O_S_B_zero, S_B_one, O_S_B_one);
        XDrawImageString(dpy,info,gc,x_str,y_str,str,strlen(str));
        y_str += spacing;
        sprintf(str,"Singularities in the North and South at %lf and %lf", 
		singularity, S_singularity);
        XDrawImageString(dpy,info,gc,x_str,y_str,str,strlen(str));
    }
    else if (mapindex == 8) {
        y_str += spacing;
        sprintf(str, 
		"Currently: pB = %8.6f ET = %8.6f FT = %8.6f CT = %8.6f GT = %8.6f",
			pB, ET, FT, CT, GT);
        XDrawImageString(dpy,info,gc,x_str,y_str,str,strlen(str));
        y_str += spacing;
        sprintf(str, "North rK = %8.4f  South rK = %8.4f", RK.x, RK.y);
        XDrawImageString(dpy,info,gc,x_str,y_str,str,strlen(str));
        y_str += spacing;
        sprintf(str,"Singularities in the North and South at %lf and %lf",
			singularity, S_singularity);
        XDrawImageString(dpy,info,gc,x_str,y_str,str,strlen(str));
    }
    else if (mapindex == 9) {
        y_str += spacing;
        sprintf(str, 
		"Currently: pB = %8.6f ET = %8.6f FT = %8.6f CT = %8.6f GT = %8.6f",
			pB, ET, FT, CT, GT);
        XDrawImageString(dpy,info,gc,x_str,y_str,str,strlen(str));
        y_str += spacing;
        sprintf(str, "North GNP = %8.4f  South GNP = %8.4f", GNP.x, GNP.y);
        XDrawImageString(dpy,info,gc,x_str,y_str,str,strlen(str));
        y_str += spacing;
        sprintf(str,"Singularities in the North and South at %lf and %lf",
			singularity, S_singularity);
        XDrawImageString(dpy,info,gc,x_str,y_str,str,strlen(str));
    }
#endif
    y_str += spacing;
    sprintf(str,"find = %d, portrait = %d, attractors = %d, critical = %d",
		find,portrait,attractors,critical);
    XDrawImageString(dpy,info,gc,x_str,y_str,str,strlen(str));
    y_str += spacing;
    if (lyap)
        sprintf(str,"lyap = %d, mandel = %d, initial (x,y) = (%lf,%lf)",
			lyap,mandel,start_x,start_y);
    else
		if (histogram)
        	sprintf(str,
				"mandel=%d intersections=%d numattrs=%d minhist=%d maxhist=%d", 
				mandel, found_arc, numattrs, minhist, maxhist);
		else
        	sprintf(str,"mandel = %d, intersections = %d, numattrs = %d", 
							mandel, found_arc, numattrs);
    XDrawImageString(dpy,info,gc,x_str,y_str,str,strlen(str));
}

MapSelect()
{
	int n;
	XEvent event;

	n = XEventsQueued(dpy, QueuedAfterFlush);
	while (n--) {
    	    XNextEvent(dpy, &event);
            switch(event.type) 
            {
            case KeyPress:
		if (KeySelect(&event))
			return(1);
		break;
            case Expose:
		Show_Maps(numselect);
	        break;
            case ButtonPress:
		if (ButtonSelect(&event))
			return(1);
	        break;
            }
	}
	return(0);
}

KeySelect(event)
XKeyEvent *event;
{
	unsigned char key;
	static int selected;
	extern int numeric;

	selected = 0;
        if (XLookupString(event, (char *)&key, sizeof(key), (KeySym *)0,
                (XComposeStatus *)0) > 0)
                switch (key) {
		case '+': dwell++; break;
		case '-': dwell--; if (dwell < 0) dwell = 0; break;
		case '}': settle++; break;
		case '{': settle--; if (settle < 0) settle = 0; break;
		case '>': dwell *= 2; if (dwell == 0) dwell = 1; break;
		case '<': dwell /= 2; break;
		case ']': settle *= 2; if (settle == 0) settle = 1; break;
		case '[': settle /= 2; break;
		case 'a':
		case 'A': portrait = (!portrait); break;
		case 'b':
		case 'B': critical = (!critical); break;
		case 'c':
		case 'C': precrit = (!precrit); break;
		case 'd':
		case 'D': histogram = (!histogram); break;
		case 'e':
		case 'f':
		case 'E':
		case 'F': if (++lyap > 2) 
				lyap = 0;
			  break;
		case '8': numselect++;
			  if (numselect >= NUMMAPS)
				numselect = 0;
			  Show_Maps(numselect);
			  break;
		case '7': numselect--;
			  if (numselect < 0)
				numselect = NUMMAPS-1;
			  break;
		case '\n':
		case ' ': selected = 1;
			  break;
		case 'q':
		case 'Q': Cleanup(); exit(0);
			  break;
		}
	Show_Maps(numselect);
	if (selected) {
		mapindex = numselect;
		map = Maps[mapindex];
		if (numeric)
			deriv = dnumeric;
		else
			deriv = Derivs[mapindex];
		reset_params();
		return(1);
	}
	return(0);
}

ButtonSelect(event)
XButtonEvent *event;
{
	static int selected;
	extern int numeric;

	selected = 0;
	switch (event->button) {
		case Button1:		/* highlight the selection */
		    if (event->x < map_width)
				numselect = Min(NUMMAPS-1, (event->y * NUMMAPS) / S_HEIGHT);
		    else if (event->x < num_width) {
				if (event->y < (S_HEIGHT/ 2))
					settle /= 2;
				else
					dwell /= 2;
		    }
		    else {
				if (event->y < (S_HEIGHT / 6))
					portrait = (!portrait);
				else if (event->y < (S_HEIGHT / 3))
					critical = (!critical);
				else if (event->y < (S_HEIGHT / 2))
					precrit = (!precrit);
				else if (event->y < (2*S_HEIGHT/3)) {
					histogram = (!histogram);
					if (histogram)
						HistoMem();
					else
						FreeHist();
				}
				else
					if (++lyap > 2)
						lyap = 0;
		    }
		    Show_Maps(numselect);
		    break;
		case Button2:		/* Used to increase dwell or settle */
		    if (event->x < map_width)
				numselect = Min(NUMMAPS-1, (event->y * NUMMAPS) / S_HEIGHT);
		    else if (event->x < num_width) {
				if (event->y < (S_HEIGHT / 2)) {
					settle *= 2; if (settle == 0) settle = 1;
				}
				else {
					dwell *= 2; if (dwell == 0) dwell = 1;
				}
		    }
		    else {
				if (event->y < (S_HEIGHT / 4))
					portrait = (!portrait);
				else if (event->y < (S_HEIGHT / 2))
					critical = (!critical);
				else if (event->y < (S_HEIGHT / 2))
					precrit = (!precrit);
				else if (event->y < (2*S_HEIGHT/3)) {
					histogram = (!histogram);
					if (histogram)
						HistoMem();
					else
						FreeHist();
				}
				else
					if (++lyap > 2)
						lyap = 0;
		    }
		    Show_Maps(numselect);
		    break;
		case Button3:		/* pick the highlighted selection */
			mapindex = numselect; selected = 1;
			break;
	}
	if (selected) {
		map = Maps[mapindex];
		if (numeric)
			deriv = dnumeric;
		else
			deriv = Derivs[mapindex];
		reset_params();
		freemem(); setupmem();
		return(1);
	}
	return(0);
}

Show_Maps(mapnum)
int mapnum;
{
	static int i, x_str, y_str;
	static GC gc;

	XFillRectangle(dpy,trajec,Data_GC[0],0,0,
	XDisplayWidth(dpy, screen), XDisplayHeight(dpy, screen));
	if (displayplanes > 1)
		XQueryTextExtents(dpy,(XID)XGContextFromGC(Data_GC[CG]),"X",
    		1,&dir,&ascent,&descent,&overall);
	else
		XQueryTextExtents(dpy,(XID)XGContextFromGC(Data_GC[1]),"X",
    		1,&dir,&ascent,&descent,&overall);
	spacing = ascent + descent + 1;
	map_width = XDisplayWidth(dpy, screen) * 0.5;
	num_width = XDisplayWidth(dpy, screen) * 0.75;
	x_str = 20; numselect = mapnum;
	for (i=0; i< NUMMAPS; i++) {
		if (i == mapnum)
			gc = Reverse_GC;
		else
    		if (displayplanes > 1)
        		gc = Data_GC[CG];
    		else
        		gc = Data_GC[1];
		y_str = (int)(((double)S_HEIGHT/(double)NUMMAPS)*((double)i+0.5));
		sprintf(str," %3d. %s ",i, Mapnames[i]);
		XDrawImageString(dpy,trajec,gc,x_str,y_str,str,strlen(str));
		if (displayplanes > 1)
			gc = Data_GC[CG];
		else
			gc = Data_GC[1];
		XDrawLine(dpy, trajec, gc, 0, i*S_HEIGHT/NUMMAPS, map_width, 
			i*S_HEIGHT/NUMMAPS);
	}
	XDrawLine(dpy, trajec, gc, 0, S_HEIGHT, map_width, S_HEIGHT);
	XDrawLine(dpy, trajec, gc, map_width, 0, map_width, S_HEIGHT);
	XDrawLine(dpy, trajec, gc, num_width-1, 0, num_width-1, S_HEIGHT);
	XDrawLine(dpy, trajec, gc, map_width, S_HEIGHT/2, num_width,S_HEIGHT/2);
	XDrawLine(dpy, trajec, gc, num_width, 0, S_WIDTH, 0);
	XDrawLine(dpy, trajec, gc, num_width, S_HEIGHT/6, S_WIDTH, S_HEIGHT/6);
	XDrawLine(dpy, trajec, gc, num_width, S_HEIGHT/3, S_WIDTH, S_HEIGHT/3);
	XDrawLine(dpy, trajec, gc, num_width, S_HEIGHT/2, S_WIDTH, S_HEIGHT/2);
	XDrawLine(dpy, trajec, gc, num_width, 2*S_HEIGHT/3, S_WIDTH, 2*S_HEIGHT/3);
	XDrawLine(dpy, trajec, gc, num_width, 5*S_HEIGHT/6, S_WIDTH, 5*S_HEIGHT/6);
	XDrawLine(dpy, trajec, gc, num_width, S_HEIGHT, S_WIDTH, S_HEIGHT);
	sprintf(str," settle = %d ", settle);
	gc = Reverse_GC;
	x_str=map_width+(((num_width-map_width)-(strlen(str)*overall.width))/2);
	y_str = (S_HEIGHT / 4);
	XDrawImageString(dpy,trajec,gc,x_str,y_str,str,strlen(str));
	sprintf(str," dwell = %d ", dwell);
	x_str=map_width+(((num_width-map_width)-(strlen(str)*overall.width))/2);
	y_str = (3 * S_HEIGHT / 4);
	XDrawImageString(dpy,trajec,gc,x_str,y_str,str,strlen(str));
	x_str = num_width; y_str = S_HEIGHT / 12;
	if (lyap)
		sprintf(str," A. Bifurcation Diagram ");
	else
		sprintf(str," A. Phase Portrait      ");
	if (portrait)
		gc = Reverse_GC;
	else
       	if (displayplanes > 1)
       		gc = Data_GC[CG];
       	else
       		gc = Data_GC[1];
	XDrawImageString(dpy,trajec,gc,x_str,y_str,str,strlen(str));
	y_str = S_HEIGHT / 4;
	sprintf(str," B. Critical Curves ");
	if (critical)
		gc = Reverse_GC;
	else
       	if (displayplanes > 1)
       		gc = Data_GC[CG];
      	else
       		gc = Data_GC[1];
	XDrawImageString(dpy,trajec,gc,x_str,y_str,str,strlen(str));
	y_str = 5 * S_HEIGHT / 12;
	sprintf(str," C. Pre-critical Curves ");
	if (precrit)
		gc = Reverse_GC;
	else
       	if (displayplanes > 1)
       		gc = Data_GC[CG];
       	else
       		gc = Data_GC[1];
	XDrawImageString(dpy,trajec,gc,x_str,y_str,str,strlen(str));
	y_str = 7 * S_HEIGHT / 12;
	sprintf(str," D. Histographic Data ");
	if (histogram)
		gc = Reverse_GC;
	else
       	if (displayplanes > 1)
       		gc = Data_GC[CG];
      	else
       		gc = Data_GC[1];
	XDrawImageString(dpy,trajec,gc,x_str,y_str,str,strlen(str));
	y_str = 3 * S_HEIGHT / 4;
	sprintf(str," E. Basins of Attraction ");
	if (!lyap)
		gc = Reverse_GC;
	else
       	if (displayplanes > 1)
       		gc = Data_GC[CG];
     	else
       		gc = Data_GC[1];
	XDrawImageString(dpy,trajec,gc,x_str,y_str,str,strlen(str));
	y_str = 11 * S_HEIGHT / 12;
	sprintf(str," F. Lyapunov Exponents ");
	if (lyap >= 2)
		gc = Reverse_GC;
	else
       	if (displayplanes > 1)
       		gc = Data_GC[CG];
       	else
       		gc = Data_GC[1];
	XDrawImageString(dpy,trajec,gc,x_str,y_str,str,strlen(str));
}

Show_Tits()
{
	static int i, x_str, y_str;
	static GC gc;
	char x_fontname[128];
	XFontStruct *x_fontinfo = (XFontStruct *)0;

	XClearWindow(dpy, trajec);
	if (displayplanes > 1)
		gc = Data_GC[CG];
	else
		gc = Data_GC[1];
        if (x_fontinfo != (XFontStruct *)0)
                XUnloadFont(dpy, x_fontinfo->fid);
	if (XDisplayWidth(dpy, screen) <= 800)
		strcpy(x_fontname, "10x20");
	else
		strcpy(x_fontname, "12x24");
	if ((x_fontinfo=XLoadQueryFont(dpy,x_fontname))!=(XFontStruct *)0){
		XSetFont(dpy, gc, x_fontinfo->fid);
		XSetFont(dpy, Reverse_GC, x_fontinfo->fid);
	}
	XQueryTextExtents(dpy,(XID)XGContextFromGC(gc),"X",
            1,&dir,&ascent,&descent,&overall);
	sprintf(str,"E N D O");
	x_str=(XDisplayWidth(dpy,screen) - (strlen(str)*overall.width))/2;
	y_str = 80;
	XDrawImageString(dpy,trajec,gc,x_str,y_str,str,strlen(str));
	if (x_fontinfo != (XFontStruct *)0)
		XUnloadFont(dpy, x_fontinfo->fid);
	if (XDisplayWidth(dpy, screen) <= 800)
		strcpy(x_fontname, "6x10");
	else
		strcpy(x_fontname, "9x15");
    if ((x_fontinfo = XLoadQueryFont(dpy,x_fontname)) != (XFontStruct *)0) {
		XSetFont(dpy, gc, x_fontinfo->fid);
		XSetFont(dpy, Reverse_GC, x_fontinfo->fid);
	}
	XQueryTextExtents(dpy,(XID)XGContextFromGC(gc),"X",
            1,&dir,&ascent,&descent,&overall);
	spacing = ascent + descent;
	y_str += 3 * spacing;
	sprintf(str,"Iterated Endomorphisms of the Plane");
	x_str=(XDisplayWidth(dpy,screen) - (strlen(str)*overall.width))/2;
	XDrawImageString(dpy,trajec,gc,x_str,y_str, str, strlen(str));
	y_str = S_HEIGHT - (5*spacing);
	sprintf(str,"by");
	x_str=(XDisplayWidth(dpy,screen) - (strlen(str)*overall.width))/2;
	XDrawImageString(dpy,trajec,gc,x_str,y_str, str, strlen(str));
	y_str += 1.25*spacing;
	sprintf(str,"Ronald Joe Record and Ralph H. Abraham");
	x_str=(XDisplayWidth(dpy,screen) - (strlen(str)*overall.width))/2;
	XDrawImageString(dpy,trajec,gc,x_str,y_str, str, strlen(str));
	y_str += 1.25 * spacing;
	sprintf(str,"Copyright (c) 1992, 1993, 1994");
	x_str=(XDisplayWidth(dpy,screen) - (strlen(str)*overall.width))/2;
	XDrawImageString(dpy,trajec,gc,x_str,y_str, str, strlen(str));
	y_str += 1.25 * spacing;
	sprintf(str, "Press space bar or any mouse button to begin");
	x_str=(XDisplayWidth(dpy,screen) - (strlen(str)*overall.width))/2;
	XDrawImageString(dpy,trajec,gc,x_str,y_str, str, strlen(str));
	y_str += 1.25 * spacing;
	sprintf(str, 
   "Press 'n' for next, 'p' for previous, 'w' for new colormap, 'q' to quit");
	x_str=(XDisplayWidth(dpy,screen) - (strlen(str)*overall.width))/2;
	XDrawImageString(dpy,trajec,gc,x_str,y_str, str, strlen(str));
}

Tit_Event()
{
	static int showem; 
	static int numdemo=0;
	static XEvent event;
	unsigned char key;
	extern int Qflag, numattrs, usedefault;
	extern void DemoSpin();

	if (run) {
		compendo();
		increment(Qflag);
	}
	else {
		if (!clearflag)
			DemoSpin(dpy, cmap, Colors, STARTCOLOR, numcolors, delay, 4);
		Change_Parms((++numdemo) % MAXDEMOS);
		Clear(trajec);
		numwheels = MAXWHEELS;
		if (!usedefault)
		    init_color(dpy, trajec, cmap, Colors, STARTCOLOR, MINCOLINDEX,
					numcolors, numwheels, "endo", "Endo", numattrs);
	}
	showem = 0;
	while (XCheckMaskEvent(dpy, KeyPressMask|ButtonPressMask|ExposureMask,
			   &event)) {
		key = 'a';
		switch(event.type) {
    		case KeyPress:
				XLookupString((XKeyEvent *)&event, (char *)&key, sizeof(key), 
					(KeySym *)0, (XComposeStatus *)0);
				if ((key == 'q') || (key == 'Q')) {
					Cleanup();
					exit(0);
				}
				else if (key == 'n') {
					numdemo = ++numdemo % MAXDEMOS;
					Change_Parms(numdemo);
					Clear(trajec);
				}
				else if (key == 'p') {
					numdemo--;
					if (numdemo < 0)
						numdemo = MAXDEMOS - 1;
					Change_Parms(numdemo);
					Clear(trajec);
				}
				else if (key == 'w') {
					if (numwheels < MAXWHEELS)
						numwheels++;
		   			else
						numwheels = 0;
					if (!usedefault)
					    init_color(dpy,trajec,cmap,Colors,STARTCOLOR,
						MINCOLINDEX,numcolors,numwheels,"endo","Endo",numattrs);
				}
				else
					return(1);
				break;
    		case ButtonPress: 
				return(1);
    		case Expose:
				if (event.xexpose.window == trajec)
					showem = 1;
	    	break;
    }
		}
	if (showem)
		Show_Tits();
	return(0);
}
