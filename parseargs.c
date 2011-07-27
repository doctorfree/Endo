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
#include <stdio.h>
#ifndef NeXT
#include <values.h>
#endif
#include "x.h"
#include "params.h"

void
parseargs(ac, av)
int ac;
char **av;
{
	static int c;
	extern short delay;
	extern int optind, color_offset, demo, negative, maxcrit, usedefault;
	extern char *optarg;
	extern double atof();
	extern void usage();

	map = Maps[0];
	deriv = Derivs[0];
	setparams(0);
	
	maxperiod=0; minperiod= MAXINT;

	while ((c=getopt(ac,av, "^+012345678:9:RUVZpquvP:T:A:B:C:D:E:F:G:I:J:K:L:M:N:Q:W:X:Y:H:O:S:a:b:c:d:e:f:g:h:i:j:k:l:m:n:o:r:s:t:w:x:y:z:"))!=EOF){
		switch (c) {
		case '^':	negative=(!negative); break;
		case '+':	axes=(!axes); break; /* overlay plot of axes */
		case '0':	demo=1; break;
		case '1':	orbits_1d=1; break; /* y bifurcation diagram */
		case '2':	orbits_1d=2; break; /* both bifurcation diag */
#ifdef USE_3D
		case '3':	orbits_3d=1; break;
#endif
		case '4':	orbits_1d=3; break; /* x bifurcation diagram */
		case '6':	animate=1; break; /* draw animation style */
		case '7':	histogram=1; break; /* draw histogram style */
		case '8':	lyap=atoi(optarg); 
				break;
				/* 0 = no Lyapunov sets */
				/* 1 = bifurcation diagrams only */
				/* 2 = bifurcation and Lyapunov sets */
				/* 3 = 2 but IC = last pt on attractor */
				/* 4 = 2 but IC = critical point */
		case '9':	mandel=atoi(optarg);
				break;
				/* 0 = no Mandelbrot/Julia sets */
				/* 1 = Mandelbrot with Lyapunov exponents */
				/* 2 = Mandelbrot in black */
				/* 3 = Rate of Attraction of critical point */
				/* 4 = Use [STARTCOLOR,MINCOLINDEX] for escapees */
		case 'A':	params[0]=atof(optarg); Aflag++; break;
		case 'B':	params[1]=atof(optarg); Bflag++; break;
		case 'C':	params[2]=atof(optarg); Cflag++; break;
		case 'D':	params[3]=atof(optarg); Dflag++; break;
		case 'E':	params[4]=atof(optarg); Eflag++; break;
		case 'F':	params[5]=atof(optarg); Fflag++; break;
		case 'G':	params[6]=atof(optarg); Gflag++; break;
		case 'H':	height=atoi(optarg);
			 	traheight=criheight=height;
				if (optind < ac)
					if (av[optind][0] == ',') {
						traheight = atoi(av[optind+1]);
						optind += 2;
					}
				if (optind < ac)
					if (av[optind][0] == ',') {
						criheight = atoi(av[optind+1]);
						optind += 2;
					}
				break;
		case 'I':	params[7]=atof(optarg); Iflag++; break;
		case 'J':	params[8]=atof(optarg); Jflag++; break;
		case 'K':	params[9]=atof(optarg); Kflag++; break;
		case 'L':	params[10]=atof(optarg); Lflag++; break;
		case 'M':	params[11]=atof(optarg); Mflag++; break;
		case 'N':	MINCOLINDEX=atoi(optarg); 
				if ((MINCOLINDEX - STARTCOLOR) <= 0) {
				  printf("An argument of -N %d ",MINCOLINDEX);
				  printf("indicates to set the minimum");
				  printf("color index to %d\n",MINCOLINDEX);
				  printf("Currently, the compiled value for");
				  printf("STARTCOLOR is %d\n",STARTCOLOR);
				  printf("The minimum color index must be greater\n");
				  exit(-1);
				}
				break;
		case 'O':	color_offset=atoi(optarg); break;
		case 'P':	critical=atoi(optarg); 
				if (critical == 1) { /* critical curves only */
					critical = 1;
					precrit = 0;
				}
				else if (critical == 2) { /* plus precritical */
					critical = 1;
					precrit = 1;
				}
				else if (critical == 3) { /* precrit only */
					critical = 0;
					precrit = 1;
				}
				else if (critical == 4) { /* pre-images of */
					precrit = 2; 	/* the diagonal only */
					critical = 0;
				}
				else if (critical == 5) { /*attractors divide */
					precrit = 3; 	/* across diagonal */
					critical = 0;
				}
				else if (critical == 6) { /* pre-images of */
					precrit = 4; 	/* the origin only */
					critical = 0;
				}
				else if (critical == 7) { /* pre-images of */
					precrit = 5; 	/* diagonal + origin */
					critical = 0; /*color origin different*/
				}
				else if (critical == 8) { /* pre-images of */
					precrit = 6; 	/* critical + origin */
					critical = 0; /*color origin different*/
				}
				else if (critical == 9) { /*attractors divide */
					precrit = 7; 	/* across y-axis */
					critical = 0;
				}
				else if (critical == 10) {/*attractors divide */
					precrit = 8; 	/* across x-axis */
					critical = 0;
				}
				else if (critical == 11) {/*rate of attraction*/
					precrit = 9; /* colored according to */
					critical = 0; /* distance from (0,0) */
				}
				else if (critical == 12) {/*rate of attraction*/
					precrit = 10; /* colored according to */
					critical = 0; /* angle with x-axis */
				}
				else if (critical == 13) {/*attractors divide */
					precrit = 11; /* across x and y axes  */
					critical = 0;
				}
				break;
		case 'Q':	p1 = atoi(optarg);
				if (optind < ac)
					p2 = atoi(av[optind++]); 
				else
					usage();
				if (optind < ac)
					if (av[optind][0] == ',') {
						Qflag = atoi(av[optind+1]);
						optind += 2;
					}
				break;
		case 'R':	randinit++; break;
		case 'S':	settle=atoi(optarg);
				if (optind < ac)
					if (av[optind][0] == ',') {
						maxcrit = atoi(av[optind+1]);
						optind += 2;
					}
				break;
		case 'T':	mapindex=atoi(optarg);
				if ((mapindex >= NUMMAPS) || (mapindex < 0))
					usage();
				map = Maps[mapindex];
				if (numeric)
					deriv = dnumeric;
				else
					deriv = Derivs[mapindex];
				if (optind < ac)
					if (av[optind][0] == ',') {
						model_1d = atoi(av[optind+1]) % 2;
						optind += 2;
					}
				setparams(mapindex);
				break;
		case 'U':	lower = 0; break;
		case 'V':	find = 1; break;
		case 'W':	width=trawidth=criwidth=atoi(optarg);
				if (optind < ac)
					if (av[optind][0] == ',') {
						trawidth = atoi(av[optind+1]);
						optind += 2;
					}
				if (optind < ac)
					if (av[optind][0] == ',') {
						criwidth = atoi(av[optind+1]);
						optind += 2;
					}
				break;
		case 'X':	start_x=atof(optarg); Xflag++; break;
		case 'Y':	start_y=atof(optarg); Yflag++; break;
		case 'Z':	upper = 0; break;
		case 'a':	params[12]=atof(optarg); aflag++; break;
		case 'b':	params[13]=atof(optarg); bflag++; break;
		case 'c':	params[14]=atof(optarg); cflag++; break;
		case 'd':	if (strcmp(optarg, "elay")) {
						if (strcmp(optarg, "efault")) {
						    params[15]=atof(optarg);
						    dflag++;
						}
						else
							usedefault = 1;
					}
					else
						delay = atoi(av[optind++]);
					break;
		case 'e':	params[16]=atof(optarg); eflag++; break;
		case 'f':	params[17]=atof(optarg); fflag++; break;
		case 'g':	params[18]=atof(optarg); gflag++; break;
		case 'h':	   y_range=atof(optarg); hflag++;
				if (optind < ac)
					if (av[optind][0] == ',') {
						t_y_range = atof(av[optind+1]);
						hflag++;
						optind += 2;
					}
				if (optind < ac)
					if (av[optind][0] == ',') {
						c_y_range = atof(av[optind+1]);
						hflag++;
						optind += 2;
					}
				break;
		case 'i':	params[19]=atof(optarg); iflag++; break;
		case 'j':	params[20]=atof(optarg); jflag++; break;
		case 'k':	params[21]=atof(optarg); kflag++; break;
		case 'l':	params[22]=atof(optarg); lflag++; break;
		case 'm':	params[23]=atof(optarg); mflag++; break;
		case 'n':	if (strcmp(optarg, "umeric"))
						if (strcmp(optarg, "umwheel"))
							dwell=atoi(optarg);
						else {
							numwheels = atoi(av[optind]);
							optind++;
						}
					else
						numeric=1; 
					break;
		case 'o':	savefile++; outname=optarg;
				if (optind < ac)
					if (av[optind][0] == ',') {
						savefile = atoi(av[optind+1]);
						optind += 2;
					}
				break;
		case 'p':	portrait = 0; break;
		case 'q':	attractors = 1; break;
		case 'r':	maxradius=atoi(optarg); break;
		case 's':	spinlength=atoi(optarg); break;
		case 't':	delta=atof(optarg); break;
		case 'u':	usage(); break;
		case 'v':	show=1; break;
		case 'w':	   x_range=atof(optarg); wflag++;
				if (optind < ac)
					if (av[optind][0] == ',') {
						t_x_range = atof(av[optind+1]);
						wflag++;
						optind += 2;
					}
				if (optind < ac)
					if (av[optind][0] == ',') {
						c_x_range = atof(av[optind+1]);
						wflag++;
						optind += 2;
					}
				break;
		case 'x':	   min_x=atof(optarg); xflag++;
				if (optind < ac)
					if (av[optind][0] == ',') {
						t_min_x = atof(av[optind+1]);
						xflag++;
						optind += 2;
					}
				if (optind < ac)
					if (av[optind][0] == ',') {
						c_min_x = atof(av[optind+1]);
						xflag++;
						optind += 2;
					}
				break;
		case 'y':	   min_y=atof(optarg); yflag++;
				if (optind < ac)
					if (av[optind][0] == ',') {
						t_min_y = atof(av[optind+1]);
						yflag++;
						optind += 2;
					}
				if (optind < ac)
					if (av[optind][0] == ',') {
						c_min_y = atof(av[optind+1]);
						yflag++;
						optind += 2;
					}
				break;
		case 'z':	cdelt=atof(optarg); break;
		case '?':	usage(); break;
		}
	}
	if (numeric)
		deriv = dnumeric;
	else
		deriv = Derivs[mapindex];
	max_x = min_x + x_range;
	max_y = min_y + y_range;
	a_minimums[0] = min_x; b_minimums[0] = min_y;
	a_maximums[0] = max_x; b_maximums[0] = max_y;
	reset_params();
}

#ifdef NorthSouth

void
calc_domain()
{
	static double tmp1, tmp2, tmp3;

	tmp3 = A_one - 1.0;
	tmp1 = -tmp3/(2.0*A_two);
	tmp2 = (1.0/(2.0*A_two))*sqrt((tmp3*tmp3) - (4.0*A_zero*A_two));
 	B_zero = tmp1 + tmp2;
 	O_B_zero = tmp1 - tmp2;
	B_one = (-A_one / A_two) - (2.0 * B_zero);
	O_B_one = (-A_one / A_two) - (2.0 * O_B_zero);
	tmp3 = S_A_one - 1.0;
	tmp1 = -tmp3/(2.0*S_A_two);
	tmp2 = (1.0/(2.0*S_A_two))*sqrt((tmp3*tmp3) - (4.0*S_A_zero*S_A_two));
	S_B_zero = tmp1 + tmp2;
	O_S_B_zero = tmp1 - tmp2;
	S_B_one = (-S_A_one / S_A_two) - (2.0 * S_B_zero);
	O_S_B_one = (-S_A_one / S_A_two) - (2.0 * O_S_B_zero);
	if (B_one < 0) {
		B_zero = B_zero + B_one;
		B_one = -B_one;
	}
	if (O_B_one < 0) {
		O_B_zero = O_B_zero + O_B_one;
		O_B_one = -O_B_one;
	}
	if (S_B_one < 0) {
		S_B_zero = S_B_zero + S_B_one;
		S_B_one = -S_B_one;
	}
	if (O_S_B_one < 0) {
		O_S_B_zero = O_S_B_zero + O_S_B_one;
		O_S_B_one = -O_S_B_one;
	}
	if (lyap) {
		if (!Xflag)
			start_x = (-1.0*A_one)/(2.0*A_two); /* the critical point */
		if (!Yflag)
		  if (model_1d)
			start_y = H_zero + (H_one * start_x);
		  else
			start_y = (-1.0*S_A_one)/(2.0*S_A_two);
	}
	else {
		if ((singularity > B_zero) && (singularity < (B_zero+B_one))) {
			if (!wflag)
				x_range = B_one - (singularity - B_zero);
			if (!xflag)
				min_x = singularity;
		}
		else {
			if (!xflag)
				min_x = B_zero; 
			if (!wflag)
				x_range = B_one;
		}
		max_x = min_x + x_range; 
		if ((S_singularity > S_B_zero) &&
			(S_singularity < (S_B_zero+S_B_one))) {
			if (!hflag)
				y_range = S_B_one - (S_singularity - S_B_zero);
			if (!yflag)
				min_y = S_singularity;
		}
		else {
			if (!yflag)
				min_y = S_B_zero;
			if (!hflag)
				y_range = S_B_one;
		}
		if ((model_1d) && (!yflag) && (!hflag)) {
			max_y = H_zero + (H_one * max_x);
			min_y = H_zero + (H_one * min_x);
			y_range = max_y - min_y;
		}
		a_minimums[frame] = min_x; b_minimums[frame] = min_y;
		a_maximums[frame] = max_x; b_maximums[frame] = max_y;
	}
}

void
calculate_params(map, p)
int map;
double *p;
{
	static double tmp1, tmp2, tmp3, tmp4;

	params[6] = (params[2]*params[5]) - (params[3]*params[4]);
	params[18] = (params[14]*params[17]) - (params[15]*params[16]);
	if (map == 7) {	/* North-South model */
		tmp1 = p[13] / p[21];
		tmp2 = 1.0 + (p[17] * p[19] / p[22]);
		tmp3 = p[20]+(p[21]*p[17]/p[18]); /* North coefficients */
		A_zero = (tmp1*tmp2*tmp3) - (p[13]*p[15]*p[15]*p[16]);
		tmp1 = (1.0-p[12]);
		tmp2 = p[13] / p[22];
		tmp3 = p[20] + (p[21]*p[17]/p[18]);
		A_one = tmp1 + (tmp2 * (-p[19] - ((p[17]/p[15]) * tmp3)));
		A_two = p[13] / p[22];
		A_star = -(p[13]*p[16]*p[16]*p[15]*p[22]/p[18]);
		singularity = p[19] + (p[14] * p[22] / p[18]);
		H_one = (p[10] * p[3] * p[18]) / (p[22] * p[15] * p[6]);
		H_zero=((p[10]/p[6])*(p[2]-(p[14]*p[3]/p[15])))-(H_one*p[19])+p[7];
		/* South quadratic coefficients and singularity */
		S_A_zero = H_zero + (H_one * A_zero) - (A_one * H_zero) +
					(A_two * H_zero * H_zero / H_one);
		S_A_one = A_one - (2.0 * A_two * H_zero / H_one);
		S_A_two = A_two / H_one;
		S_A_star = H_one * H_one * A_star;
		S_singularity = H_zero + (H_one * singularity);
		calc_domain();
	}
}
#endif /* NorthSouth */

setparams(map)
int map;
{
	if (!Aflag)
		params[0] = defparms[map][0];
	if (!Bflag)
		params[1] = defparms[map][1];
	if (!Cflag)
		params[2] = defparms[map][2];
	if (!Dflag)
		params[3] = defparms[map][3];
	if (!Eflag)
		params[4] = defparms[map][4];
	if (!Fflag)
		params[5] = defparms[map][5];
	if (!Gflag)
		params[6] = defparms[map][6];
	if (!Iflag)
		params[7] = defparms[map][7];
	if (!Jflag)
		params[8] = defparms[map][8];
	if (!Kflag)
		params[9] = defparms[map][9];
	if (!Lflag)
		params[10] = defparms[map][10];
	if (!Mflag)
		params[11] = defparms[map][11];
	if (!aflag)
		params[12] = defparms[map][12];
	if (!bflag)
		params[13] = defparms[map][13];
	if (!cflag)
		params[14] = defparms[map][14];
	if (!dflag)
		params[15] = defparms[map][15];
	if (!eflag)
		params[16] = defparms[map][16];
	if (!fflag)
		params[17] = defparms[map][17];
	if (!gflag)
		params[18] = defparms[map][18];
	if (!iflag)
		params[19] = defparms[map][19];
	if (!jflag)
		params[20] = defparms[map][20];
	if (!kflag)
		params[21] = defparms[map][21];
	if (!lflag)
		params[22] = defparms[map][22];
	if (!mflag)
		params[23] = defparms[map][23];
	if (!xflag)
		min_x = amins[map];
	if (!wflag)
		x_range = aranges[map];
	if (!yflag)
		min_y = bmins[map];
	if (!hflag)
		y_range = branges[map];
	max_x = min_x + x_range;
	max_y = min_y + y_range;
	a_minimums[frame] = min_x; b_minimums[frame] = min_y;
	a_maximums[frame] = max_x; b_maximums[frame] = max_y;
	if (lyap) {
		params[p1] = pmins[map][p1];
		params[p2] = pmins[map][p2];
		if (!xflag)
			min_x = pmins[map][p1];
		if (!yflag)
			min_y = pmins[map][p2];
		if (!wflag)
			max_x = pmaxs[map][p1];
		if (!hflag)
			max_y = pmaxs[map][p2];
		x_range = max_x - min_x;
		y_range = max_y - min_y;
		a_minimums[frame] = min_x; b_minimums[frame] = min_y;
		a_maximums[frame] = max_x; b_maximums[frame] = max_y;
	}
#ifdef NorthSouth
	if (map == 7)
		calculate_params(map, params);
	else {
#endif
		if (!Xflag)
			start_x = definit[map][0];
		if (!Yflag)
			start_y = definit[map][1];
#ifdef NorthSouth
	}
#endif
}

reset_params()
{
	extern int found_arc;

	if (mapindex >= 0)
		setparams(mapindex);
	x_inc = x_range / (double)width;
	y_inc = y_range / (double)height;
	point.x = 0; point.y = 0;
	maxexp = minexp = 0;
	run = 1;
	x = min_x; y = min_y;
	numcrits = 0; numarcs = 0; perind[frame] = 0; found_arc = 0;
	if (xflag < 2)
		t_min_x = min_x;
	if (yflag < 2)
		t_min_y = min_y;
	if (xflag < 3)
		c_min_x = min_x;
	if (yflag < 3)
		c_min_y = min_y;
	if (wflag < 2)
		t_x_range = x_range;
	if (hflag < 2)
		t_y_range = y_range;
	if (wflag < 3)
		c_x_range = x_range;
	if (hflag < 3)
		c_y_range = y_range;
}

Save_Parms(state)
int state;
{
	static int C, F, P, S, n, Q0, Q1, Q2, TW, TH, W, H, nw, a, h, L, i;
	static double x0, x1, w0, w1, y0, y1, h0, h1, tmp_params[MAXPARAMS];

	freemem(); 
	if (state) {
		for (i=0; i<MAXPARAMS; i++)
			tmp_params[i] = params[i];
		C = critical; P = portrait; S = settle; L = lyap; F = find;
		n = dwell; 
		Q0 = p1; Q1 = p2; Q2 = Qflag;
		x0 = min_x; x1 = t_min_x; 
		w0 = x_range; w1 = t_x_range; 
		y0 = min_y; y1 = t_min_y; 
		h0 = y_range; h1 = t_y_range; 
		W = width; H = height; 
		TW = trawidth; TH = traheight; 
		nw = numwheels; a = animate; h = histogram;
	}
	else {
		for (i=0; i<MAXPARAMS; i++)
			params[i] = tmp_params[i];
		map = Maps[mapindex]; 
		if (numeric)
			deriv = dnumeric;
		else
			deriv = Derivs[mapindex];
		settle = S; dwell = n; Qflag = Q2;
		p1 = Q0; p2 = Q1;
		lyap = L; critical = C; find = F; portrait = P;
		min_x = x0; t_min_x = x1; x_range = w0; t_x_range = w1;
		min_y = y0; t_min_y = y1; y_range = h0; t_y_range = h1;
		width = criwidth = W; 
		height = criheight = H;
		trawidth = TW; 
		traheight = TH;
		numwheels = nw; animate = a; histogram = h;
		xflag = yflag = wflag = hflag = 0;
		reset_params();
	}
	setupmem();
}

void
Change_Parms(num)
int num;
{
	extern long lrand48();
	extern double drand48();

	if (histogram)
		FreeHist();
	AllInitBuffer();
	run = 1; settle = DEMO_SETTLE; dwell = DEMO_DWELL;
	animate = lrand48() % 2;
	if (animate) 
		clearflag = 0;
	else
		clearflag = lrand48() % 2;
	lyap = 1; find = critical = 0; portrait = 1;
	histogram = orbits_1d = precrit = 0;
	if (XDisplayWidth(dpy, screen) < 800) {
		width = DEMO_16_WIDTH; trawidth = DEMO_16_WIDTH; 
		height = DEMO_16_HEIGHT; traheight = DEMO_16_WIDTH;
	}
	else {
		width = DEMO_WIDTH; trawidth = DEMO_WIDTH; 
		height = DEMO_HEIGHT; traheight = DEMO_WIDTH;
	}
	xflag = yflag = wflag = hflag = 2;
	mapindex = 8 + MAP_OFF; 
	p1 = 0; p2 = 1; Qflag = 0;
	if (num == 0) {
		min_x = 5.175781; t_min_x = -1.303307;
		x_range = 1.289063; t_x_range = 2.664537;
		y_range = 0.433594; t_y_range = 3.642277;
		min_y = 1.082031; t_min_y = -1.375339;
	}
	else if (num == 1) {
		mapindex = 9+MAP_OFF; Qflag = lrand48() % 4; clearflag = 0;
		animate = 1;
	  	min_x = (drand48()/2.0) + 0.2; t_min_x = 0.0; t_x_range = 1.0;
 	 	min_y = (drand48()/2.0) + 0.2; t_min_y = 0.0; t_y_range = 1.0;
		x_range = 1.0 - min_x; y_range = 1.0 - min_y;
	}
	else if (num == 2) {
	  	min_x = 6.0; t_min_x = -1.617188;
 	 	x_range = 6.0; t_x_range = 3.328126;
 	 	y_range = 1.0; t_y_range = 5.478515;
 	 	min_y = 2.0; t_min_y = -1.597656;
	}
	else if (num == 3) {
		mapindex = 2+MAP_OFF; 
		p1 = 8; p2 = 10; Qflag = 3;
	  	min_x = 0.0; t_min_x = 0.0;
 	 	x_range = 1.0; t_x_range = 1.0;
 	 	y_range = 1.0; t_y_range = 1.0;
 	 	min_y = 0.0; t_min_y = 0.0;
	}
	else if (num == 4) {
		p1 = 1; p2 = 0;
	  	min_x = 0.465057; t_min_x = -1.556885;
 	 	x_range = 2.390259; t_x_range = 3.155823;
 	 	y_range = 6.869888; t_y_range = 8.365632;
 	 	min_y = 6.621113; t_min_y = -2.969666;
	}
	else if (num == 5) {
		mapindex = 9+MAP_OFF; 
		histogram = 1; animate = 0;
	  	min_x = 0.660602; t_min_x = 0.491563;
 	 	x_range = 0.064783; t_x_range = 0.548026;
 	 	y_range = 0.01; t_y_range = 0.825685;
 	 	min_y = 0.638281; t_min_y = 0.044144;
	}
	else if (num == 6) {
	  	min_x = 114.988281; t_min_x = -25.442434;
 	 	x_range = 22.841563; t_x_range = 51.060763;
 	 	y_range = 17.1875; t_y_range = 95.178377;
 	 	min_y = 37.742; t_min_y = -20.741033;
	}
	else if (num == 7) {
	  	min_x = 131.0; t_min_x = -25.44325;
 	 	x_range = 2.0; t_x_range = 51.06325;
 	 	y_range = 0.22; t_y_range = 92.178;
 	 	min_y = 36.65; t_min_y = -20.741;
	}
	else if (num == 8) {
		mapindex = 9+MAP_OFF; 
		histogram = lrand48() % 2;
	  	min_x = 0.779; t_min_x = 0.0;
 	 	x_range = 0.2053; t_x_range = 1.0;
 	 	if (clearflag)
			y_range = 0.1523; 
		else
			y_range = 0.01; 
		t_y_range = 1.0;
 	 	min_y = 0.418; t_min_y = 0.0;
		orbits_1d = 2; animate = 0;
	}
	else if (num == 9) {
		mapindex = 12+MAP_OFF;
		lyap = 0; histogram = 1; animate = 0;
	  	min_x = 0.0; t_min_x = -10.0;
 	 	x_range = 1.0; t_x_range = 21.0;
 	 	y_range = 1.0; t_y_range = 21.0;
 	 	min_y = 0.0; t_min_y = -10.0;
		params[0] = (drand48()/10.0) - 0.3; Aflag = 1;
		params[1] = (drand48()/500.0) + 0.007; Bflag = 1;
		params[2] = (drand48()/5.0) - 0.3; Cflag = 1;
	}
	else {
	  	min_x = 5.8; t_min_x = -1.475;
 	 	x_range = 20.0; t_x_range = 2.984375;
 	 	y_range = 3.0; t_y_range = 4.25;
 	 	min_y = 1.8; t_min_y = -1.509375;
	}
	if (histogram)
		HistoMem();
	map = Maps[mapindex]; 
	if (numeric)
		deriv = dnumeric;
	else
		deriv = Derivs[mapindex];
	reset_params();
	mapindex = -1;
}
