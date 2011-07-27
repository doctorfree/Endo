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

#define MAX_HISTEX 4
#define MIN_HISTEX -4
#define ADELTA 0.05
#define BDELTA 0.1

int cornerbar = 1;
static int ishow=1;

extern void init_color(), recalc(), print_help(), Save_Parms();
extern void HistoMem(), FreeHist(), CritiMem(), FreeCrit(), rehist();
extern void go_init(), go_back(), go_down(), Clear(), Draw_Axes();
extern void save_to_file(), Redraw(), redraw(), Show_Maps(); 
extern void Redraw_Hist(), Draw_Diagonal(), flood(), rebasin(), fillbar();
#ifdef USE_3D
extern void SetWindow(), TranslateWindow(), ScaleWindow(); 
extern void TranslateView();
#endif

void
dump_colmap() {
    FILE *map;
	int a, b, c, i;
	char buf[255];

    sprintf(buf,"colormap%d.map",numwheels);
    if ((map = fopen(buf, "w")) == NULL)
        perror(buf);
    else {
		for (i=0; i<numcolors; i++) {
            a = Colors[i].red / 256;
            b = Colors[i].green / 256;
            c = Colors[i].blue / 256;
            sprintf(buf,"%d %d %d\n",a,b,c);
            fputs(buf, map);
        }
    }
}

void
GetSpecial(event, c)
XKeyEvent *event;
unsigned char c;
{
	static int set_sea=1;
	static int i;
	static double tx;
	extern int numeric, axes, see_domain, model_1d, orbits_1d, updt, adown;
	extern int orbit_in_basin, pdown, orbit_in_precrit, numattrs, _numdivs;
	extern int maxcrit, histex, maxhist, minhist, start, nonleg, cdown;
	extern int orbit_in_crit, usedefault;
	extern short delay;
	extern double A, B, C, D, sky_level, sea_level;

	switch (c) {
	case '\022': dump_colmap();		/* ctrl-R dumps the color map to a file */
		  break;
	case '\023': run = 0;		/* ctrl-S pauses computation */
		  break;
	case '\021': run = 1;		/* ctrl-Q resumes computation */
		  break;
	case '\033': /* ctrl-[ */
			if (adjcol)
				drawmap(A, B, C, D);
			A -= ADELTA*A;	/* ctrl-[ moves (A,m*B) in */
			if (A < 0.0) A=0.0; /* get_hist_index() to left */
			if (adjcol)
				drawmap(A, B, C, D);
			Redraw_Hist();
			break;
	case '\035': /* ctrl-] */
			if (adjcol)
				drawmap(A, B, C, D);
			A += ADELTA*(1.0-A);	/* ctrl-] moves (A,m*B) in */
			if (A > 1.0) A=1.0; 		/* get_hist_index() to right */
			if (adjcol)
				drawmap(A, B, C, D);
			Redraw_Hist();
			break;
	case '\036': /* ctrl-^ */
			if (adjcol)
				drawmap(A, B, C, D);
			B += BDELTA*(1.0-B);	/* ctrl-^ moves (A,m*B) in */
			if (B > 1.0) B=1.0; 		/* get_hist_index() upward */
			if (adjcol)
				drawmap(A, B, C, D);
			Redraw_Hist();
			break;
	case '\037': /* ctrl-_ */
			if (adjcol)
				drawmap(A, B, C, D);
			B -= BDELTA*B;	/* ctrl-_ moves (A,m*B) in */
			if (B < 0.0) B=0.0; /* get_hist_index() downward */
			if (adjcol)
				drawmap(A, B, C, D);
			Redraw_Hist();
			break;
	case '\001': /* ctrl-A toggles setting sea_level and sky_level */
			set_sea = (!set_sea);
			break;
	case '1': model_1d = (!model_1d);
		  break;
	case '2': orbits_1d++;
		  if (orbits_1d >= 4)
			orbits_1d = 0;
		  if (!usedefault)
		      init_color(dpy, trajec, cmap, Colors, STARTCOLOR, MINCOLINDEX,
					numcolors, numwheels, "endo", "Endo", numattrs);
		  Clear(trajec);
		  break;
#ifdef USE_3D
	case '3': orbits_3d=(!orbits_3d);
		  Clear(trajec);
		  break;
#endif
	case '4': see_domain = (!see_domain);
		  break;
	case '0': if (event->window == trajec) { /* Zoom out, doubling edges */
			t_min_x -= t_x_range/2;
			t_min_y -= t_y_range/2;
			t_x_range += t_x_range;
			t_y_range += t_y_range;
		  }
		  else if (event->window == crijec) {
			c_min_x -= c_x_range/2;
			c_min_y -= c_y_range/2;
			c_x_range += c_x_range;
			c_y_range += c_y_range;
		  }
		  else {
			min_x -= x_range/2;
			min_y -= y_range/2;
			x_range += x_range;
			y_range += y_range;
			newframe();
		  }
		  SetFocus(event->window);
		  break;
	case '5': if (event->window == trajec) /* Pan Left by half the width */
			t_min_x -= t_x_range/2.0;
		  else if (event->window == crijec)
			c_min_x -= c_x_range/2.0;
		  else {
			min_x -= x_range/2.0;
			newframe();
		  }
		  SetFocus(event->window);
		  break;
	case '6': if (event->window == trajec) /* Pan Right by half the width */
			t_min_x += t_x_range/2.0;
		  else if (event->window == crijec)
			c_min_x += c_x_range/2.0;
		  else {
			min_x += x_range/2.0;
			newframe();
		  }
		  SetFocus(event->window);
		  break;
	case '7': if (event->window == trajec) /* Pan down by half the height */
			t_min_y -= t_y_range/2.0;
		  else if (event->window == crijec)
			c_min_y -= c_y_range/2.0;
		  else {
			min_y -= y_range/2.0;
			newframe();
		  }
		  SetFocus(event->window);
		  break;
	case '8': if (event->window == trajec) /* Pan up by half the height */
			t_min_y += t_y_range/2.0;
		  else if (event->window == crijec)
			c_min_y += c_y_range/2.0;
		  else {
			min_y += y_range/2.0;
			newframe();
		  }
		  SetFocus(event->window);
		  break;
	case '9': if (event->window == trajec) { /* Zoom in to center quarter */
			t_min_x += t_x_range/4;
			t_min_y += t_y_range/4;
			t_x_range -= t_x_range/2;
			t_y_range -= t_y_range/2;
		  }
		  else if (event->window == crijec) {
			c_min_x += c_x_range/4;
			c_min_y += c_y_range/4;
			c_x_range -= c_x_range/2;
			c_y_range -= c_y_range/2;
		  }
		  else {
			min_x += x_range/4;
			min_y += y_range/4;
			x_range -= x_range/2;
			y_range -= y_range/2;
			newframe();
		  }
		  SetFocus(event->window);
		  break;
	case '<': dwell /= 2; if (dwell < 1) dwell = 1; break;
	case '>': dwell *= 2;
		  freecoords();
		  setupcoords();
		  break;
	case '(': delay -= 25; if (delay < 0) delay=0; break;
	case ')': delay += 25; break;
	case '[': settle /= 2; if (settle < 1) settle = 0; break;
	case ']': settle *= 2; if (settle < 1) settle = 1; break;
	case '{': delta /= 2.0; break;
	case '}': delta *= 2.0; break;
	case '-': cdelt /= 2.0; break;
	case '+': cdelt *= 2.0; break;
	case '^': 
	case '%':
			if (portrait && histogram)
		  		Redraw_Hist(); break;
#ifdef USE_3D
	case '.': SetWindow(3);
		  Clear(trajec);
		  break;
	case ',': SetWindow(2);
		  Clear(trajec);
		  break;
#endif
	case '/': Draw_Diagonal(event->window);
		  break;
	case '|': axes = (!axes);
		  Draw_Axes(event->window, axes);
		  break;
	case '#': numeric = (!numeric);
		  if (numeric)
			deriv = dnumeric;
		  else
		  	deriv = Derivs[mapindex];
		  break;
	case '\002': /* ctrl-B  toggles color bar display */
			showbar++;
			if (showbar > 2)
				showbar = 0;
			if (showbar == 1) {
				XMapRaised(dpy, hisbar);
				if (!usedefault)
		  		    init_color(dpy, hisbar, cmap, Colors, STARTCOLOR, MINCOLINDEX,
					numcolors, numwheels, "endo", "Endo", numattrs);
				fillbar(hisbar, HISBARHGT, HISBARHGT, cornerbar);
			}
			else if (showbar == 2) {
				fillbar(trajec, traheight/2, traheight, cornerbar);
				XUnmapWindow(dpy, hisbar);
			}
			else {
				if (cornerbar)
					XFillRectangle(dpy, pixtra, Data_GC[0], 
						0, traheight/2, HISBARWID, traheight/2);
				else
					XFillRectangle(dpy, pixtra, Data_GC[0], 
						0, 0, HISBARWID, traheight/2);
				XCopyArea(dpy, pixtra, trajec, Data_GC[0],
					0, 0, trawidth, traheight, 0, 0);
			}
			break;
	case '\003': /* (ctrl-C) toggle drawing trajectory in critical window */
		  orbit_in_crit = (!orbit_in_crit);
		  if (orbit_in_crit) {
			if (cdown) {
				XMapRaised(dpy, crijec);
				stacktop = crijec;
				if (!usedefault)
		  		    init_color(dpy,crijec,cmap,Colors,STARTCOLOR,MINCOLINDEX,
							numcolors,numwheels,"endo","Endo", numattrs);
				cdown=0;
			}
		  }
		  break;
	case '\005':  /* (ctrl-E) recalculate colors for basins */
		  rebasin();
		  redraw(indices[frame], perind[frame], 1);
		  ishow=0; break;
	case '\011':  /* (ctrl-I) toggles color bar display algorithm */
			nonleg = (!nonleg);
			if (showbar == 1)
				fillbar(hisbar, HISBARHGT, HISBARHGT, cornerbar);
			else if (showbar == 2)
				fillbar(trajec, traheight/2, traheight, cornerbar);
			break;
	case '\006':  /* (ctrl-F) set sea or sky level to 1.0 */
		if (set_sea) {
		  if (sea_level != 1.0) {
			  tx = sea_level;
			  sea_level = 1.0;
			  i = maxhist - minhist;
			  flood(1, 1, (int)(tx*(double)i)+minhist, maxhist);
		  }
		}
		else {
		  if (sky_level != 1.0) {
			  tx = sky_level;
			  sky_level = 1.0;
			  i = maxhist - minhist;
			  flood(0, 1, (int)(tx*(double)i)+minhist, MAXINT);
		  }
		}
		  break;
	case '\007':  /* (ctrl-G) set sea level back to 0.0 */
		if (set_sea)
		  if (sea_level) {
			  tx = sea_level;
			  sea_level = 0.0;
			  i = maxhist - minhist;
			  flood(1, 0, (int)(tx*(double)i)+minhist, 1);
		  }
		else
		  if (sky_level) {
			  tx = sky_level;
			  sky_level = 0.0;
			  i = maxhist - minhist;
			  flood(0, 0, (int)(tx*(double)i)+minhist, 1);
		  }
		  break;
	case '\010':  /* (ctrl-H) decrement sea level by lesser of color, hit */
		  tx = Min((1.0 / (double)(numcolors - start - 1)),
				   (1.0 / (double)(maxhist - minhist)));
		if (set_sea) {
		  sea_level -= tx;
		  if (sea_level < 0.0)
			  sea_level = 0.0;
		  i = (int)(sea_level * (double)(maxhist - minhist)) + minhist;
		  flood(1, 0, i + (int)(tx*(double)(maxhist - minhist)), 
				Max(1,i - (int)(tx*(double)(maxhist - minhist))));
		}
		else {
		  sky_level -= tx;
		  if (sky_level < 0.0)
			  sky_level = 0.0;
		  i = (int)(sky_level * (double)(maxhist - minhist)) + minhist;
		  flood(0, 0, i + (int)(tx*(double)(maxhist - minhist)), 
				Max(1,i - (int)(tx*(double)(maxhist - minhist))));
		}
		  break;
	case '\012':  /* (ctrl-J) increment sea level by lesser of color, hit */
		  tx = Min((1.0 / (double)(numcolors - start - 1)),
				   (1.0 / (double)(maxhist - minhist)));
		if (set_sea) {
		  sea_level += tx;
		  if (sea_level > 1.0)
			  sea_level = 1.0;
		  i = (int)(sea_level * (double)(maxhist - minhist)) + minhist;
		  flood(1, 1, i - (int)(tx*(double)(maxhist - minhist)), i);
		}
		else {
		  sky_level += tx;
		  if (sky_level > 1.0)
			  sky_level = 1.0;
		  i = (int)(sky_level * (double)(maxhist - minhist)) + minhist;
		  flood(0, 1, i - (int)(tx*(double)(maxhist - minhist)), i);
		}
		  break;
	case '\013':  /* (ctrl-K) decrement histogram algorithm index */
		  if (adjcol)
			drawmap(A, B, C, D);
		  histex--;
		  if (histex < MIN_HISTEX)
			  histex = MAX_HISTEX;
		  if (adjcol)
			drawmap(A, B, C, D);
		  rehist();
		  if (showbar == 1)
			fillbar(hisbar, HISBARHGT, HISBARHGT, cornerbar);
		  else if (showbar == 2)
			fillbar(trajec, traheight/2, traheight, cornerbar);
		  break;
	case '\014':  /* (ctrl-L) increment histogram algorithm index */
		  if (adjcol)
			drawmap(A, B, C, D);
		  histex++;
		  if (histex > MAX_HISTEX)
			  histex = MIN_HISTEX;
		  if (adjcol)
			drawmap(A, B, C, D);
		  rehist();
		  if (showbar == 1)
			fillbar(hisbar, HISBARHGT, HISBARHGT, cornerbar);
		  else if (showbar == 2)
			fillbar(trajec, traheight/2, traheight, cornerbar);
		  break;
	case '\015': /* ctrl-M */ 
		  maxcrit /= 2; break;
	case '\016': /* ctrl-N */ 
		  maxcrit *= 2; if (maxcrit < 1) maxcrit = 1; break;
	case '\017': /* ctrl-O toggles corner in which color bar is drawn */ 
		  cornerbar = (!cornerbar); break;
	case '\024':  /* (ctrl-T) toggle drawing trajectory in precrit window */
		  orbit_in_precrit = (!orbit_in_precrit);
		  if (orbit_in_precrit) {
			if (pdown) {
				XMapRaised(dpy, prejec);
				stacktop = prejec;
				if (!usedefault)
		    	    init_color(dpy,prejec,cmap,Colors,STARTCOLOR,MINCOLINDEX,
							numcolors,numwheels,"endo","Endo", numattrs);
				pdown=0;
			}
		  }
		  break;
	case '\020': 	/* (ctrl-P) toggle drawing trajectory in basin */
		  orbit_in_basin = (!orbit_in_basin);
		  if (orbit_in_basin) {
			if (adown) {
				XMapRaised(dpy, canvas);
				stacktop = canvas;
				if (!usedefault)
		    	    init_color(dpy,canvas,cmap,Colors,STARTCOLOR,MINCOLINDEX,
							numcolors,numwheels,"endo","Endo", numattrs);
				adown=0;
			}
		  }
		  break;
	case '\025': /* ctrl-U toggles scale display in color window */
			updt = (!updt); break;
	case '\026': adjcol = (!adjcol);	/* ctrl-V toggles color adjust window */
			if (adjcol) {
				XMapRaised(dpy, colwin);
				if (!usedefault)
		    	    init_color(dpy,colwin,cmap,Colors,STARTCOLOR,MINCOLINDEX,
							numcolors,numwheels,"endo","Endo", numattrs);
				drawmap(A, B, C, D);
			}
			else
				XUnmapWindow(dpy, colwin);
		  break;
	case '\027':  /* (ctrl-W) set color palette to black & white */
			if (numwheels == -2)
				numwheels = -1; /* all white (for debug purposes) */
			else
				numwheels = -2;
			if (!usedefault)
		   	    init_color(dpy,stacktop,cmap,Colors,STARTCOLOR,MINCOLINDEX,
						numcolors,numwheels,"endo","Endo", numattrs);
		   ishow=0; break;
	case '\030':  /* (ctrl-X) decrease divisions in rainbow palette */
		   if (_numdivs > 1)
			_numdivs--;
		   else
			_numdivs = MAXDIVS;
			if (!usedefault)
		   	    init_color(dpy,stacktop,cmap,Colors,STARTCOLOR,MINCOLINDEX,
						numcolors,numwheels,"endo","Endo", numattrs);
		    ishow=0; break;
	case '\031':  /* (ctrl-Y) increase divisions in rainbow palette */
		   if (_numdivs < MAXDIVS)
			_numdivs++;
		   else
			_numdivs = 1;
			if (!usedefault)
		   	    init_color(dpy,stacktop,cmap,Colors,STARTCOLOR,MINCOLINDEX,
						numcolors,numwheels,"endo","Endo", numattrs);
		   ishow=0; break;
	}
}

void
Getkey(event)
XKeyEvent *event;
{
	unsigned char key;
	XKeyEvent *keyevent = (XKeyEvent *)event;
	static int i, dorecalc=0, olyap=-1; 
	static int spinning=0, spindir=0;
	static pair t0;
	static double tx, ty;
	static triple tvector;
	static XWindowChanges values;
	static XWindowAttributes attr;
	extern short delay;
	extern int found_arc, negative, lowrange, axes, orbits_1d;
	extern int pdown, cdown, adown, tdown, ldown, idown;
	extern int model_1d, animate, mandel, numeric, numattrs;
	extern int first, second, begin, middle, see_domain, usedefault;
	extern triple window_center; 
	extern double window_distance, avg();

	ishow = 1;
	if (XLookupString(keyevent, (char *)&key, sizeof(key), (KeySym *)0,
		(XComposeStatus *)0) > 0)
	switch (key) {
	case 'a': attractors = (!attractors); break;
	case 'A': if (critical) remap_arcs(); ishow=0; break;
	case 'b': if (which == canvas) {
				ldown = (!ldown);
				if (ldown)
					XUnmapWindow(dpy,lyajec);
				else {
					XMapRaised(dpy, lyajec);
					stacktop = lyajec;
					if (!usedefault)
		  			    init_color(dpy,lyajec,cmap,Colors,STARTCOLOR,MINCOLINDEX,
								numcolors,numwheels,"endo","Endo", numattrs);
				}
			  }
			  else {
				adown = (!adown);
				if (adown)
					XUnmapWindow(dpy,canvas);
				else {
					XMapRaised(dpy, canvas);
					stacktop = canvas;
					if (!usedefault)
		  			    init_color(dpy,canvas,cmap,Colors,STARTCOLOR,MINCOLINDEX,
								numcolors,numwheels,"endo","Endo", numattrs);
				}
			  }
			  ishow=0; break;
	case 'B': adown = (!adown);
		  ldown = (!ldown);
		  if (ldown) {
			XUnmapWindow(dpy,lyajec);
		  }
		  else {
			XMapRaised(dpy, lyajec);
			stacktop = lyajec;
			if (!usedefault)
		  	    init_color(dpy,lyajec,cmap,Colors,STARTCOLOR,MINCOLINDEX,
							numcolors,numwheels,"endo","Endo", numattrs);
		  }
		  if (adown) {
			XUnmapWindow(dpy,canvas);
		  }
		  else {
			XMapRaised(dpy, canvas);
			stacktop = canvas;
			if (!usedefault)
		  	    init_color(dpy,canvas,cmap,Colors,STARTCOLOR,MINCOLINDEX,
							numcolors,numwheels,"endo","Endo", numattrs);
		  }
		  ishow=0; break;
	case 'c': Clear(trajec); ishow=0; break;
	case 'C': cdown = (!cdown);
		  if (cdown) {
			XUnmapWindow(dpy,crijec);
			critical = 0;
		  }
		  else {
			XMapRaised(dpy, crijec);
			stacktop = crijec;
			if (!usedefault)
		  	    init_color(dpy,crijec,cmap,Colors,STARTCOLOR,MINCOLINDEX,
							numcolors,numwheels,"endo","Endo", numattrs);
			critical = 1;
		  }
		  if (critical)
			CritiMem();
		  else
			FreeCrit();
		  ishow=0; break;
	case 'd': go_down(); break;
	case 'D': AllFlushBuffer(); ishow=0; break;
	case 'e': animate = (!animate); break;
	case 'E': AllFlushBuffer();
		  dorecalc = (!dorecalc);
		  rehist();
		  if (dorecalc)
			recalc(); 
		  else {
			if (lyap) {
				for (i=0;i<perind[frame];i++) {
					if (indices[frame][i] < MINCOLINDEX) {
						indices[frame][i] = (periods[frame][i] 
											% lowrange) + STARTCOLOR;
					}
					else {
						indices[frame][i] = (periods[frame][i] 
											% (numfreecols-1)) + MINCOLINDEX;
					}
				}
			}
			else {
				for (i=0;i<perind[frame];i++) {
				  if ((!find) || (basins[frame][i])) {
					if (periods[frame][i] < 0) {
				   		indices[frame][i] = (ABS(periods[frame][i])
											% first) + begin;
					}
					else {
					  if (periods[frame][i])
				   		indices[frame][i] = (periods[frame][i]
											% second) + middle;
					}
				  }
				}
			}
		  }
		  redraw(indices[frame], perind[frame], 1);
		  ishow=0; break;
	case 'f': find = (!find);
		  break;
	case 'F': save_to_file(pixmap, width, height); ishow=0; break;
	case 'g': if (critical) Remap_arcs(crijec); ishow=0; break;
	case 'G': if (portrait) Remap_arcs(trajec); ishow=0; break;
	case 'H': histogram = (!histogram); 
			if (histogram)
				HistoMem();
			else
				FreeHist();
		  Clear(trajec);
		  if (lyap)
			XStoreName(dpy, trajec, BIF_NAME);
		  else if (histogram)
			XStoreName(dpy, trajec, HIST_NAME);
		  else
			XStoreName(dpy, trajec, TRAJ_NAME);
		  break;
	case '?':
	case 'h': XGetWindowAttributes(dpy, help, &attr);
		  if (attr.map_state != IsUnmapped)
			XUnmapWindow(dpy, help);
		  else {
			XMapRaised(dpy, help);
		  	print_help();
		  }
		  ishow=0; break;
	case 'i': randinit = (!randinit);
		  ishow=0; break;
	case 'I': if (idown) {
			XMapRaised(dpy, info);
			stacktop = info;
			if (!usedefault)
		  	    init_color(dpy,info,cmap,Colors,STARTCOLOR,MINCOLINDEX,
							numcolors,numwheels,"endo","Endo", numattrs);
			idown = 0;
		  }
		  else {
			XUnmapWindow(dpy, info);
			idown = 1;
		  }
		  break;
	case 'j': clearflag = (!clearflag);
		  ishow=0; break;
	case 'J': run = 1; mapindex = -1;
		  Save_Parms(1); Change_Parms(0);
		  XMapRaised(dpy, lyajec);
		  if (!usedefault)
		      init_color(dpy,lyajec,cmap,Colors,STARTCOLOR,MINCOLINDEX,
							numcolors,numwheels,"endo","Endo", numattrs);
		  ldown=0; Clear(lyajec);
		  which = lyajec;
		  values.x = 15; values.y = 15;
		  values.border_width = 5;
		  values.width = XDisplayWidth(dpy, screen);
		  values.height = XDisplayHeight(dpy, screen);
		  XConfigureWindow(dpy,trajec,
			CWBorderWidth|CWX|CWY|CWWidth|CWHeight,&values);
		  Title_Page();
		  values.x = Max((XDisplayWidth(dpy,screen)/2)-(width/2),0);
		  values.y = Max(INFO_HEIGHT,(XDisplayHeight(dpy,screen)/2) -
			(height / 2)) + 30;
		  values.width = width;
		  values.height = height;
		  XConfigureWindow(dpy,trajec,
			CWBorderWidth|CWX|CWY|CWWidth|CWHeight,&values);
		  if (lyap)
			  XStoreName(dpy, trajec, BIF_NAME);
		  else if (histogram)
			  XStoreName(dpy, trajec, HIST_NAME);
		  else
			  XStoreName(dpy, trajec, TRAJ_NAME);
		  Raise_Maps();
		  break;
#ifdef USE_3D
	case 'k': tvector.x = (window_center.x - view_point.x) / 2.0;
		  tvector.y = (window_center.y - view_point.y) / 2.0;
		  tvector.z = (window_center.z - view_point.z) / 2.0;
		  TranslateView(tvector);
		  Clear(trajec);
		  break;
	case 'K': tvector.x = (view_point.x - window_center.x) * 2.0;
		  tvector.y = (view_point.y - window_center.y) * 2.0;
		  tvector.z = (view_point.z - window_center.z) * 2.0;
		  TranslateView(tvector);
		  Clear(trajec);
		  break;
#endif
	case 'L': /* toggle drawing of bifurcation diagrams */
		  /* with and without lyapunov exponents */
		  if (olyap == -1)
			olyap = lyap;	/* save original setting */
		  lyap = (!lyap);
		  if (lyap && olyap)	/* restore positive setting to */
			lyap = olyap;	/* original if it was positive */
		  if (lyap >= 2) {
			if (ldown) {
				XMapRaised(dpy, lyajec);
				stacktop = lyajec;
				Clear(lyajec);
				ldown=0;
			}
			if (!usedefault)
		        init_color(dpy,lyajec,cmap,Colors,STARTCOLOR,MINCOLINDEX,
							numcolors,numwheels,"endo","Endo", numattrs);
			which = lyajec;
		  }
		  if (lyap == 1) {
			XUnmapWindow(dpy,lyajec);
			ldown = 1;
		  }
		  break;
	case 'l': if (olyap == -1)
			olyap = lyap;	/* save original setting */
		  lyap = (!lyap);
		  if (lyap && olyap)	/* restore positive setting to */
			lyap = olyap;	/* original if it was positive */
		  if (lyap) {
			if (ldown) {
				XMapRaised(dpy, lyajec);
				stacktop = lyajec;
				Clear(lyajec);
				ldown=0;
			}
			if (!usedefault)
		        init_color(dpy,lyajec,cmap,Colors,STARTCOLOR,MINCOLINDEX,
							numcolors,numwheels,"endo","Endo", numattrs);
			which = lyajec;
			reset_params();
			XStoreName(dpy, trajec, BIF_NAME);
		  }
		  else {
			if (adown) {
				XMapRaised(dpy, canvas);
				stacktop = canvas;
				if (!usedefault)
		    	    init_color(dpy,canvas,cmap,Colors,STARTCOLOR,MINCOLINDEX,
							numcolors,numwheels,"endo","Endo", numattrs);
				Clear(canvas);
				adown=0;
			}
			which = canvas;
			tx = x; ty = y;
			reset_params();
			params[p1] = tx; params[p2] = ty;
			if (histogram)
				XStoreName(dpy, trajec, HIST_NAME);
			else
				XStoreName(dpy, trajec, TRAJ_NAME);
		  }
		  if (portrait)
			Clear(trajec);
		  break;
	case 'M': mandel++;
		  if (mandel > 4) mandel = 0; break;
	case 'm': mapindex++; 
		  if (mapindex >= NUMMAPS) 
			mapindex=0;
		  else if (mapindex < 0)
			mapindex = NUMMAPS - 1;
		  map = Maps[mapindex];
		  if (numeric)
		  	deriv = dnumeric;
		  else
		  	deriv = Derivs[mapindex];
		  reset_params();
		  break;
	case 'N': pdown = (!pdown);
		  if (pdown) {
			XUnmapWindow(dpy,prejec);
			if (precrit != 3) precrit = 0;
		  }
		  else {
			XMapRaised(dpy, prejec);
			stacktop = prejec;
			if (!usedefault)
		        init_color(dpy,prejec,cmap,Colors,STARTCOLOR,MINCOLINDEX,
							numcolors,numwheels,"endo","Endo", numattrs);
			if (precrit != 3) precrit = 1;
		  }
		  break;
	case 'n': negative = (!negative);
		  AllFlushBuffer();
		  ishow=0; break;
#ifdef USE_3D
	case 'o': tvector.x = avg(0);
		  tvector.y = avg(1);
		  tvector.z = ((params[p1]-min_x)/x_range) * window_distance;
		  TranslateWindow(tvector);
		  ScaleWindow(0.5, 0.5);
		  Clear(trajec);
		  break;
	case 'O': ScaleWindow(2.0, 2.0);
		  tvector.x = -avg(0);
		  tvector.y = -avg(1);
		  tvector.z = -((params[p1]-min_x)/x_range) * window_distance;
		  TranslateWindow(tvector);
		  Clear(trajec);
		  break;
#endif
	case 'P':
	case 'p': 	/* toggle trajectory display */
		  tdown = (!tdown);
		  if (tdown) {
			portrait = 0;
			XUnmapWindow(dpy, trajec);
		  }
		  else {
			XMapRaised(dpy, trajec);
			stacktop = trajec;
			if (!usedefault)
		        init_color(dpy,trajec,cmap,Colors,STARTCOLOR,MINCOLINDEX,
							numcolors,numwheels,"endo","Endo", numattrs);
			Clear(trajec);
			portrait = 1;
		  }
		  break;
	case 'q':
	case 'Q': Clear(trajec); Clear(which); 
		  Clear(crijec);
		  Cleanup();
		  exit(0); break;
	case 'r': AllFlushBuffer(); redraw(indices[frame], perind[frame], 1); 
		  ishow=0; break;
	case 'R': AllFlushBuffer(); Redraw(); break;
	case 's': spinning=(!spinning); 
			if (spinning) {
				spindir=(!spindir);
				Spin(dpy, cmap, Colors, STARTCOLOR, numcolors, delay, spindir); 
			}
			ishow=0; break;
	case 'S': run=0;
		  XSelectInput(dpy,trajec,KeyPressMask|ButtonPressMask|
				ButtonMotionMask|ButtonReleaseMask|
				StructureNotifyMask|FocusChangeMask);
		  values.x = 15;
		  values.y = 15;
		  values.border_width = 5;
		  values.width = XDisplayWidth(dpy, screen);
		  values.height = XDisplayHeight(dpy, screen);
		  XConfigureWindow(dpy,trajec,
			CWBorderWidth|CWX|CWY|CWWidth|CWHeight,&values);
		  XStoreName(dpy, trajec, SELECT_NAME);
		  XMapRaised(dpy, trajec);
		  if (!usedefault)
		      init_color(dpy,trajec,cmap,Colors,STARTCOLOR,MINCOLINDEX,
							numcolors,numwheels,"endo","Endo", numattrs);
		  Show_Maps(mapindex);
		  for(;;)
			if (MapSelect())
				break;
		  values.x = Max((XDisplayWidth(dpy,screen)/2)-(width/2),0);
		  values.y = Max(INFO_HEIGHT,(XDisplayHeight(dpy,screen)/2) -
			(height / 2)) + 30;
		  values.width = width;
		  values.height = height;
		  XConfigureWindow(dpy,trajec,
			CWBorderWidth|CWX|CWY|CWWidth|CWHeight,&values);
		  if (lyap)
			  XStoreName(dpy, trajec, BIF_NAME);
		  else if (histogram)
			  XStoreName(dpy, trajec, HIST_NAME);
		  else
			  XStoreName(dpy, trajec, TRAJ_NAME);
		  XSelectInput(dpy,trajec,KeyPressMask|ButtonPressMask|
			ButtonMotionMask| ButtonReleaseMask|ExposureMask|
			StructureNotifyMask|FocusChangeMask);
		  if (portrait) {
		  	XMapRaised(dpy, trajec);
			tdown = 0;
		  	stacktop = trajec;
			if (!usedefault)
		        init_color(dpy,trajec,cmap,Colors,STARTCOLOR,MINCOLINDEX,
							numcolors,numwheels,"endo","Endo", numattrs);
		  	Clear(trajec);
		  }
		  break;
	case 't': 
			if (found_arc >= 2) {
				find_arcs(B0, B1); 
				t0 = B0; B0 = A0; A0 = t0;
				t0 = B1; B1 = A1; A1 = t0;
		  	}
			else if (found_arc == 1) {
				fprintf(stderr,"Only one intersection found\n");
				find_arcs(A0, A1); 
			}
			else
				fprintf(stderr,"%d intersections detected\n",found_arc);
			ishow=0; break;
	case 'T': save_to_file(pixtra, trawidth, traheight); ishow=0; break;
	case 'u': go_back(); break;
	case 'U': go_init(); break;
	case 'v': save_to_file(pixpre, width, height); ishow=0; break;
	case 'V': save_to_file(pixcri, criwidth, criheight); ishow=0; break;
	case 'W': if (numwheels < MAXWHEELS)
				numwheels++;
		   	  else
				numwheels = 0;
			  if (!usedefault)
		   	      init_color(dpy,stacktop,cmap,Colors,STARTCOLOR,MINCOLINDEX,
						numcolors,numwheels,"endo","Endo", numattrs);
		      ishow=0; break;
	case 'w': if (numwheels > 0)
				numwheels--;
			  else
				numwheels = MAXWHEELS;
			  if (!usedefault)
			      init_color(dpy,stacktop,cmap,Colors,STARTCOLOR,MINCOLINDEX,
						numcolors,numwheels,"endo","Endo", numattrs);
		      ishow=0; break;
	case 'x': Clear(which); ishow=0; break;
	case 'X': Destroy_frame(); break;
	case 'y': if (critical) Remap_crit(crijec); ishow=0; break;
	case 'Y': if (portrait) Remap_crit(trajec); ishow=0; break;
	case 'z': Clear(crijec); Clear(prejec); ishow=0; break;
	case 'Z': if (critical) remap_crit(); ishow=0; break;
	default:  GetSpecial(event, key);
			  break;
	}
	if ((!idown) && (ishow))
		Show_Info();
	if (spinning)
		Spin(dpy, cmap, Colors, STARTCOLOR, numcolors, delay, spindir); 
}

newframe()
{
	extern int Qflag, p_inc, q_inc;

	frame = (maxframe + 1) % MAXFRAMES;
	if (frame > maxframe)
		maxframe = frame;
	a_minimums[frame] = min_x;
	b_minimums[frame] = min_y;
	x_inc = x_range / (double)width;
	y_inc = y_range / (double)height;
	a_maximums[frame] = max_x = min_x + x_range;
	b_maximums[frame] = max_y = min_y + y_range;
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
	if (Qflag == 1) {
		if (x < y) {
			if (y < max_x)
			x = y;
			else
			run = 0;
		}
		else {
			if (x < max_y)
			y = x;
			else
			run = 0;
		}
	}
	perind[frame] = 0;;
	Clear(which); Clear(prejec);
}

double
avg(index)
int index;
{
	static int i;
	static double a;

	a = 0.0;
	for (i=0; i<dwell; i++)
		a += coords[frame][index][i];
	if (dwell)
		a /= dwell;
	return(a);
}
