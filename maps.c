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
#ifndef NeXT
#include <values.h>
#endif
#include "defines.h"

double
wrap(z, a, b)
double z;
double a, b;
{
	double w;

	w = z;
	while (w > b) 
		w -= b - a;
	while (w < a) 
		w += b - a;
	return(w);
}

double
trace(x, y, p)
double x, y;
double *p;
{
	static pair c, cx, cy;
	static double r;
	extern PFP map;
	extern double delta;

	c = (*map)(x, y, p);
	cx = (*map)(x+delta, y, p);
	cy = (*map)(x, y+delta, p);

	r = (cx.x + cy.y - c.x - c.y) / delta;
	return(r);
}

pair
gaertner(x, y, p)		/* the Gaertner "logistic" map */
double x, y; 
double *p;
{
	pair coord;

	coord.x=(p[0]*x*(p[4] - (p[5]*x))) + (p[1]*y*(p[6] - (p[7]*y)));
	coord.y=(p[2]*y*(p[8] - (p[9]*y))) + (p[3]*x*(p[10] - (p[11]*x)));
	return(coord);
}

pair
dgaertner(x, y, p)
double x, y;
double *p;
{
	pair r;
	static double AC, BD;

	AC = p[0] * p[2]; BD = p[1] * p[3];
	/* calculate the determinant */
	r.x = (AC*p[4]*p[8])-(BD*p[6]*p[10]);
	r.x -= 2.0*((AC*p[5]*p[8])+(BD*p[6]*p[11]))*x;
	r.x -= 2.0*((AC*p[4]*p[9]) + (BD*p[7]*p[10]))*y;
	r.x += 4.0*((AC*p[5]*p[9]) - (BD*p[7]*p[11]))*x*y;
	/* calculate the trace */
	r.y = (p[0]*p[4]) + (p[2]*p[8]) - (2.0*((p[0]*p[5]*x) + (p[2]*p[9]*y)));
	return(r);
}

pair
goodwin(x, y, p)			/* the Goodwin map */
double x, y; 
double *p;
{
	pair coord;

	if (p[0] == -x) {
		coord.y = y;
		coord.x = x;
		return(coord);
	}
	coord.y = ((1.0 / (p[0] + x)) - (p[3] + p[4])) * y;
	if (coord.y == 1.0) {
		coord.y = y;
		coord.x = x;
		return(coord);
	}
	coord.x = (1.0 + p[1] * ((1.0 / (1.0 - coord.y)) - p[2]) - p[3]) * x;
	return(coord);
}

pair
dgoodwin(x, y, p)
double x, y;
double *p;
{
	pair r;
	double s;
	static double A, B, C, D, E, yn1, sqy, aplusx, sqa;

	r.x = r.y = MAXDOUBLE;
	A = p[0]; B = p[1]; C = p[2]; D = p[3]; E = p[4];
	/* calculate the determinant */
	if (A == -x)
		return(r);
	yn1 = 1.0 - (y/(A+x)) + (D*y) + (E*y); aplusx = A + x;
	if ((aplusx == 0.0) || (yn1 == 0.0))
		return(r);
	sqy = yn1 * yn1; sqa = aplusx * aplusx;
	r.x = 1.0 + ((((B*yn1) - (B*x*y/sqa)))/sqy) -(B*C) - D;
	r.x = r.x * ((1.0/aplusx) - D - E);
	s = (y/sqa)*(B*x*((-1.0/aplusx) + D + E))/sqy;
	r.x = r.x - s;
	/* calculate the trace later */
	r.y = trace(x, y, p);
	return(r);
}

pair
gucken(x, y, p)		/* the Guckenhiemer, Oster, Ipaktchi map */
double x, y; 
double *p;
{
	pair coord;
	extern double exp();

	coord.x = ((p[0] * x) + (p[1] * y)) * exp(-p[2] * (x + y));
	coord.y = p[3] * x;
	return(coord);
}

pair
dgucken(x, y, p)
double x, y;
double *p;
{
	static pair r;
	static double s, t;
	extern double exp();

	s = p[2]*((p[0]*x) + (p[1]*y));
	t = exp(-p[2]*(x+y));
	/* calculate the determinant */
	r.x = -p[3] * ((p[1] - s) * t);
	/* calculate the trace */
	r.y = (p[0] - s) * t;
	return(r);
}

pair
standard(x, y, p) /* the Standard map (see Wood, Lichtenberg & Lieberman) */
double x, y; 
double *p;
{
	static pair coord;
	static double s;
	extern double sin();

	s = sin(y);
	coord.x = wrap(x + (p[0] * s), 0.0, M_2PI);
	coord.y = wrap(y + x + (p[1] * s), 0.0, M_2PI);
	return(coord);
}

pair
dstandard(x, y, p)
double x, y;
double *p;
{
	pair r;
	extern double cos();

	/* calculate the determinant */
	r.x = 1.0 + ((p[1] - p[0]) * cos(y));
	/* calculate the trace later */
	r.y = trace(x, y, p);
	return(r);
}

pair
hump(x, y, p)		/* the "double sine hump" map on the plane */
double x, y; 
double *p;
{
	pair coord;
	static double sx, sy;
	extern double sin();

	sx = sin(M_PI * x);
	sy = sin(M_PI * y);
	coord.x = wrap((p[0] * sx) + (p[1] * sy), 0.0, 2.0);
	coord.y = wrap((p[2] * sy) + (p[3] * sx), 0.0, 2.0);
	return(coord);
}

pair
dhump(x, y, p)
double x, y;
double *p;
{
	pair r;
	extern double cos();

	/* calculate the determinant */
	r.x = ((p[0]*p[2])-(p[1]*p[3])) * (M_PI*M_PI*cos(M_PI*x)*cos(M_PI*y));
	/* calculate the trace later */
	r.y = trace(x, y, p);
	return(r);
}

pair
circle(x, y, p)		/* the "double circle" map on the plane */
double x, y; 
double *p;
{
	static pair c;
	static double sx, sy;
	extern double sin();

	sx = sin(M_2PI * x);
	sy = sin(M_2PI * y);
	c.x=wrap((p[4]+(p[8]*x)-(p[0]*sx))+(p[5]+(p[9]*y)-(p[1]*sy)),0.0,1.0);
	c.y=wrap((p[6]+(p[10]*y)-(p[2]*sy))+(p[7]+(p[11]*x)-(p[3]*sx)),0.0,1.0);
	return(c);
}

pair
dcircle(x, y, p)
double x, y;
double *p;
{
	pair r;
	static double cy, cx;
	extern double cos();

	cy = cos(M_2PI*wrap(y, 0.0, 1.0)); cx = cos(M_2PI*wrap(x, 0.0, 1.0));
	/* calculate the determinant */
	r.x = ((p[8]*p[10])-(p[9]*p[11]))+(((p[1]*p[11])-(p[2]*p[8]))*M_2PI*cy);
	r.x += ((p[3]*p[9]) - (p[0]*p[10])) * M_2PI * cx;
	r.x += ((p[0]*p[2])-(p[1]*p[3]))*4.0*M_2PI*M_2PI* cx * cy;
	/* calculate the trace later */
	r.y = trace(x, y, p);
	return(r);
}

pair
circle2(x, y, p)	/* a 2nd "double circle" map on the plane */
double x, y; 
double *p;
{
	pair coord;
	extern double sin(), cos();

	coord.x = wrap((p[4]+(p[8]*x)-(p[0]*sin(M_2PI*x))) +
			(p[5]+(p[9]*y)-(p[1]*cos(M_2PI*y))), 0.0, 1.0);
	coord.y = wrap((p[6]+(p[10]*y)-(p[2]*sin(M_2PI*y))) +
			(p[7]+(p[11]*x)-(p[3]*cos(M_2PI*x))), 0.0, 1.0);
	return(coord);
}

pair
dcircle2(x, y, p)
double x, y;
double *p;
{
	pair r;
	static double cy, cx, sy, sx;
	extern double cos(), sin();

	cy = cos(M_2PI*y); cx = cos(M_2PI*x);
	sy = sin(M_2PI*y); sx = sin(M_2PI*x);
	/* calculate the determinant */
	r.x = (p[9]*p[11])-(p[8]*p[10])+(p[1]*p[11]*M_2PI*sy)+(p[2]*p[8]*M_2PI*cy);
	r.x += (p[3]*p[9]*M_2PI*sx) + (p[0]*p[10]*M_2PI*cx);
	r.x += (p[1]*p[3]*4.0*M_2PI*M_2PI*sx*sy)-(p[0]*p[2]*4.0*M_2PI*M_2PI*cx*cy);
	/* calculate the trace later */
	r.y = trace(x, y, p);
	return(r);
}

#ifdef Gardini

pair
gard(x, y, p) /* Gardini figure 8 from Barugola, A. & Cathala, J. C. */
double x, y;   /* "Annular Chaotic Areas", Nonlinear Analysis, Theory, */
double *p;     /* Methods, & Applications Vol 10, no 11, pp. 1223-1236, 1986 */
{
	static pair coord;

	coord.x = y - (p[1]*x*y);	/* p[1] = 2 */
	coord.y = (y - (p[0]*x)) + (p[2]*x*x*x); /* p[2] = 2 */
	return(coord);
}

pair
dgard(x, y, p)
double x, y;
double *p;
{
	static pair r;

	r.x = (3.0*p[1]*p[2]*x*x*x) - (p[1]*p[0]*x) - (p[1]*y);
	r.x += (3.0*p[2]*x*x) -p[0];
	r.y = 1.0 - (p[1]*y);
	return(r);
}

pair
gard2(x, y, p) /* Gardini fig 18 from Cathala, J.C. "On Some Properties */
double x, y;    /* of Absorbtive Areas in Second Order Endomorphisms" in the */
double *p;      /* 1989 European Conference on Iteration Theory, pp. 42-54 */
{
	static pair coord;

	coord.x = (p[1]*x) + y;		/* 0 < b < 0.01 */
	coord.y = p[0] + (x*x);		/* a = -0.7 */
	return(coord);
}

pair
dgard2(x, y, p)
double x, y;
double *p;
{
	static pair r;

	r.x = -2.0 * x;
	r.y = p[1];
	return(r);
}

pair
gard3(x, y, p) /* Gardini figure 20 unknown source */
double x, y; 
double *p;
{
	static pair coord;

	coord.x = p[1] * y;	/* p[1] = 1 */
	coord.y = y - (p[0]*x) + (x*x);
	return(coord);
}

pair
dgard3(x, y, p)
double x, y;
double *p;
{
	static pair r;
	
	r.x = (p[0] - (2.0*x))*p[1];
	r.y = 1.0;
	return(r);
}

#endif /* Gardini */

double
poly(x, p)
double x;
double *p;
{
	static int i;
	static double r, s;

	s = 1.0;
	r = 0.0;
	for  (i=0; i<p[0]; i++) {
		r += p[i+1]*s;
		s *= x;
	}
	return(r);
}

double
dpoly(x, p)
double x;
double *p;
{
	static int i;
	static double r, s;

	s = 1.0;
	r = 0.0;
	for  (i=1; i<p[0]; i++) {
		r += i*p[i+1]*s;
		s *= x;
	}
	return(r);
}

pair
alexander(x, y, p)	/* Alexander map described in Science News 11/14/92 */
double x, y;
double *p;
{
	pair z;

	z.x = (x*x) - (y*y) - x - (p[0]*y);
	z.y = (2.0*x*y) - (p[1]*x) + y;
	return(z);
}

pair
dalexander(x, y, p)
double x, y;
double *p;
{
	pair r;

	r.x = (4.0*((x*x) + (y*y))) + ((p[0]-p[1])*2.0*y) - (p[0]*p[1]) - 1.0;
	r.y = (4.0*x);
	return(r);
}

pair
secant(x, y, p)	/* secant method for f(x) = ax^n + bx^n-1 + ... + px + q */
double x, y;
double *p;
{
	pair z;
	static double fx, fy;
	extern double delta;

	fx = poly(x, p);
	fy = poly(y, p);
	if (ABS(fx) < delta)
		z.x = x;
	else {
		if (fx != fy)
			z.x = x - ((fx * (x - y)) / (fx - fy));
	    else
			z.x = MAXDOUBLE;
	}
	z.y = x;
	return(z);
}

pair
dsecant(x, y, p)
double x, y;
double *p;
{
	static double fx, fy, s;
	static pair r;

	fx = poly(x, p);
	fy = poly(y, p);
	s = (fx - fy);
	/* calculate the determinant */
	if (s == 0.0)
		r.x = MAXDOUBLE;
	else
		r.x = (-fx*(s - (fx*(x-y)*dpoly(y,p))))/(s*s);
	/* calculate the trace later */
	r.y = trace(x, y, p);
	return(r);
}

/* From the book "Symmetry In Chaos" by Michael Field and Martin Golubitsky.
 * Written as a map of the real plane :
 *     F(x,y) = m(x,y) + (u,v) + A(sin(2*Pi*x), sin(2*Pi*y))
 *            + B(sin(2*Pi*x)*cos(2*Pi*y), sin(2*Pi*y)*cos(2*Pi*x))
 *            + C(sin(4*Pi*x), sin(4*Pi*y))
 *            + D(sin(6*Pi*x)*cos(4*Pi*y), sin(6*Pi*y)*cos(4*Pi*x))
 *            + E(sin(2*Pi*y), sin(2*Pi*x))
 * In addition, i allow the use of a different set of parameters for x and y.
 * where A, B, C, D, E, m, u and v are all real parameters.
 */
pair
golub(x, y, p)
double x, y;
double *p;
{
	pair z;
	static double sx, sy, cy, cx, fpx, fpy;
	extern double sin(), cos();

	sx = sin(M_2PI * x); sy = sin(M_2PI * y);
	cx = cos(M_2PI * x); cy = cos(M_2PI * y);
	fpx = 2.0 * M_2PI * x; fpy = 2.0 * M_2PI * y;
	z.x = (p[6]*x) + p[5] + (p[0]*sx) + (p[1]*sx*cy) + (p[2]*sin(fpx));
	z.x += ((p[3]*sin(3.0*M_2PI*x)*cos(fpy)) - (p[4]*sy));
	z.y = (p[18]*y) + p[17] + (p[12]*sy) + (p[13]*sy*cx) + (p[14]*sin(fpy));
	z.y += ((p[15]*sin(3.0*M_2PI*y)*cos(fpx)) - (p[16]*sx));
	return(z);
}

pair
dgolub(x, y, p)	/* Not Done Yet - use the "-numeric" argument */
double x, y;
double *p;
{
	extern pair dnumeric();

	return(dnumeric(x, y, p));
}

/* From the book "Symmetry In Chaos" by Michael Field and Martin Golubitsky.
 * With additional parameters "n" and "p", the exponent and power, as doubles.
 * Written as a map of the complex plane :
 *     F(z) = (A+B*z*zbar+C*Re(z^n)+F*Re((z/|z|)^np)*|z|+Ei)*z + D*(zbar^(n-1))
 * where A, B, C, D, E, F, p and n are all real parameters, z =x+iy complex.
 * This is a combination of their two "symmetric  icon" formulas. In this
 * more general form, i've kept the omega term to allow Zn symmetry and added
 * the non-polynomial term to allow introduction of a singularity at the origin.
 * If you want to get the first symmetric icon formula from the book, then just
 * set F=0 above. If you want the second one from the book, then set E=0.
 */
pair
marty(x, y, p)
double x, y;
double *p;
{
	pair t, z, w;
	static double s, c;
	extern void cmul(), zbar(), zpow(), cadd();
	extern double rpzn(), sqrt();

	if (p[6] == 0.0)
		s = 0.0;
	else {
		c = sqrt(x*x+y*y);
		if (c == 0.0)
			s = 0.0;
		else
			s = p[6]*rpzn(x/c,y/c,p[5]*p[7])*c;
	}
	cmul(&z,p[0]+(p[1]*((x*x)+(y*y)))+(p[2]*rpzn(x,y,p[5]))+s,p[4],x,y);
	zbar(&w, x, y);
	zpow(&t, w.x, w.y, p[5] - 1.0);
	t.x *= p[3]; t.y *= p[3];
	cadd(&w, z.x, z.y, t.x, t.y);
	return(w);
}

pair
dmarty(x, y, p)	/* Not Done Yet - use the "-numeric" argument */
double x, y;
double *p;
{
	extern pair dnumeric();

	return(dnumeric(x, y, p));
}

pair
rotor(x, y, p) /* the rotor map (see Yorke's dynamics software) */
double x, y; 
double *p;
{
	static pair coord;
	static double s;
	extern double sin();

	s = sin(wrap(x, 0.0, M_2PI));
	if (p[1] == 0.0) {
		coord.y = y;
		coord.x = x;
		return(coord);
	}
	coord.y = (y - (p[0] * s))/p[1];
	coord.x = wrap((x - coord.y), -M_PI, M_PI);
	if (p[1] == 1.0)
		coord.y = wrap(coord.y, -M_PI, M_PI);
	return(coord);
}

pair
drotor(x, y, p)
double x, y;
double *p;
{
	pair r;

	r.x = r.y = MAXDOUBLE;
	/* calculate the determinant */
	if (p[1] == 0.0)
		return(r);
	r.x = 1.0 / p[1];
	r.y = trace(x, y, p);
	return(r);
}

pair
twistandflip(x, y, p) /* the Twist and Flip map (see Brown and Chua) */
double x, y; 
double *p;
{
	static pair coord;
	static double s, c, theta, xmina;
	extern double cos(), sqrt(), sin();

	xmina = x - p[1];
	theta = (M_PI / p[0]) * sqrt((xmina*xmina) + (y*y));
	s = sin(wrap(theta, 0.0, M_2PI));
	c = cos(wrap(theta, 0.0, M_2PI));
	coord.x = -1.0 * ((xmina * c) - (y * s) + p[1]);
	coord.y = -1.0 * ((xmina * s) + (y * c));
	return(coord);
}

pair
dtwistandflip(x, y, p)
double x, y;
double *p;
{
	pair r;
	static double s, c, theta, t, xmina, poverw, yoverr; 
	static double dxdx, dxdy, dydy, dydx;
	extern double cos(), sqrt(), sin();

	xmina = x - p[1];
	poverw = M_PI / p[0];
	t = sqrt((xmina*xmina) + (y*y));
	yoverr = y / t;
	theta = poverw * t;
	s = sin(wrap(theta, 0.0, M_2PI));
	c = cos(wrap(theta, 0.0, M_2PI));
	dxdx = ((-1.0) * poverw * ((xmina / t) * ((y*c) - (xmina*s)))) - c;
	dydx = ((-1.0) * poverw * ((xmina / t) * ((xmina*c) - (y*s)))) - s;
	dxdy = (poverw * yoverr *(xmina*s + (y*c))) + s;
	dydy = ((-1.0) * poverw * yoverr *(xmina*c + (y*s))) - c;
	/* calculate the determinant and trace of the Jacobian */
	r.x = (dxdx*dydy) - (dydx*dxdy);
	r.y = dxdx + dydy;
	return(r);
}

double
Fmira(x, p)
double x, p;
{
	static double z;

	z = x*x;
	return(p*x + ((2.0*(1.0-p)*z)/(1.0 + z)));
}

pair
mira(x, y, p)		/* From Transformations Ponctuelles et Leurs */
double x, y;		/* Applications (1973) by Bernussou, Hsu & Mira */
double *p;
{
	pair coord;

	coord.x= y + Fmira(x, p[0]) + (p[1]*y*(1 + (p[2]*y*y)));
	coord.y= Fmira(coord.x, p[0]) - x;
	return(coord);
}

double
Fdmira(x, p)
double x, p;
{
	static double z;

	return((4.0*(1.0-p)*x)/((1.0+z)*(1.0+z)));
}

pair
dmira(x, y, p)
double x, y;
double *p;
{
	static double a, b, c, d;
	pair coord, r;

	coord = mira(x, y, p);
	a = p[0] + Fdmira(x, p[0]);
	b = 1.0 + p[1] + (3.0*p[1]*p[2]*y*y);
	c = (Fdmira(coord.x, p[0]) * Fdmira(x, p[0])) - 1.0;
	d = p[0] + Fdmira(coord.x, p[0]);
	/* calculate the determinant of the jacobian */
	r.x = b*((a*d) - c);
	/* calculate the trace later */
	r.y = trace(x, y, p);
	return(r);
}

pair
lorenz(x, y, p)		/* From Physica D 35 (1989) 299-317 by Lorenz */
double x, y; 
double *p;
{
	pair coord;

	coord.x= ((1.0 + (p[1]*p[0]))*x) - (p[0]*x*y);
	coord.y= ((1.0 - p[0]) * y) + (p[0]*x*x);
	return(coord);
}

pair
dlorenz(x, y, p)
double x, y;
double *p;
{
	static double a, b, c;
	pair r;

	a = 1.0 + (p[0]*p[1]) - (p[0]*y);
	b = p[0]*x;
	c = 1.0 - p[0];
	/* calculate the determinant */
	r.x = (a*c) - (2.0*b*b);
	/* calculate the trace */
	r.y = a + c;
	return(r);
}

pair
logistic(x, y, p)		/* the "double logistic" map */
double x, y; 
double *p;
{
	pair coord;

	coord.x=(4.0*p[0]*x*(1.0 - x)) + ((1.0 - p[0]) * y);
	coord.y=(4.0*p[1]*y*(1.0 - y)) + ((1.0 - p[1]) * x);
	return(coord);
}

pair
dlogistic(x, y, p)
double x, y;
double *p;
{
	pair r;
	double A, B;

	A = p[0]; B = p[1];
	/* calculate the determinant */
	r.x = ((16.0*A*B)*(1.0 - (2.0*y))*(1.0 - (2.0*x))) - ((1.0-A)*(1.0-B));
	/* calculate the trace */
	r.y = (4.0*(p[0] + p[1])) - (8.0*((p[0]*x)+(p[1]*y)));
	return(r);
}

pair
dorband(x, y, p)		/* the Dorband "double logistic" map */
double x, y; 
double *p;
{
	pair coord;

	coord.x=(4.0*p[0]*y*(1.0 - y)) + ((1.0 - p[0]) * x);
	coord.y=(4.0*p[1]*x*(1.0 - x)) + ((1.0 - p[1]) * y);
	return(coord);
}

pair
ddorband(x, y, p)
double x, y;
double *p;
{
	pair r;

	/* calculate the determinant */
	r.x=((1.0-p[0])*(1.0-p[1]))-((16.0*p[0]*p[1])*(1.0-(2.0*y))*(1.0-(2.0*x)));
	/* calculate the trace */
	r.y = 2.0 - p[0] - p[1];
	return(r);
}

double
fpei(x,y, p)
double x, y; 
double *p;
{
	return((p[3]*x) - (p[4]*x*y));
}

double
gpei(x, y, p)
double x, y;
double *p;
{
	return((p[6]*x*y) - (p[5]*y));
}

double
ppei(x,y,p)
double x, y;
double *p;
{
	return(x + (p[1]*fpei(x,y,p)));
}

double
qpei(x,y,p)
double x, y;
double *p;
{
	return(y + (p[2]*gpei(x,y,p)));
}

pair
peitgen(x, y, p)	/* From "The Beauty of Fractals" */
double x, y;		/* by Peitgen et al , page 125 */
double *p;
{
	pair coord;

	coord.x=x+((p[0]*0.5)*(fpei(x,y,p)+fpei(ppei(x,y,p),qpei(x,y,p),p)));
	coord.y=y+((p[0]*0.5)*(gpei(x,y,p)+gpei(ppei(x,y,p),qpei(x,y,p),p)));
	return(coord);
}

pair
dpeitgen(x, y, p)
double x, y;
double *p;
{
	static double hover2, pfx, pfy, pgx, pgy, a, b, c, d;
	pair r;

printf("");
	hover2 = p[0] * 0.5;
	a = (p[2]*p[4]*p[5])-(2.0*p[1]*p[3]*p[4])+(p[1]*p[2]*p[3]*p[4]*p[5]);
	b = (p[2]*p[4]*p[6]) + (p[1]*p[2]*p[3]*p[4]*p[6]);
	c = p[1]*p[2]*p[4]*p[4]*p[5]; 
	d = p[1]*p[2]*p[4]*p[4]*p[6];
	pfx = p[3] - (p[4]*y) + (p[3]+(p[1]*p[3]*p[3])) + (a*y) - 
			(2.0*b*x*y) - (c*y*y) + (2.0*d*x*y*y);
	pfy = (p[1]*p[4]*p[4]) - (p[4]*x) + (a*x) - (b*x*x) - (2.0*c*x*y) + 
			(2.0*d*x*x*y);
	a=p[6]-(2.0*p[2]*p[6]*p[5])+(p[1]*p[3]*p[6])-(p[1]*p[2]*p[3]*p[6]*p[5]);
	b = (p[2]*p[6]*p[6]) + (p[1]*p[2]*p[3]*p[6]*p[6]);
	c = p[1]*p[2]*p[4]*p[6]*p[5]; 
	d = p[1]*p[2]*p[4]*p[6]*p[6];
	pgx = (p[6]*y) + (a*y) + (2.0*b*x*y) + (c*y*y) - (2.0*d*x*y*y);
	pgy = (p[6]*x) - (2.0*p[5]) + (p[2]*p[5]*p[5]) + (a*x) + (b*x*x) + 
			(2.0*c*x*y) - (2.0*d*x*x*y);
	a = 1.0 + (hover2 * pfx);
	b = hover2 * pfy;
	c = hover2 * pgx;
	d = 1.0 + (hover2 * pgy);
	/* calculate the determinant of the jacobian */
	r.x = (a*d) - (b*c);
	/* calculate the trace */
	r.y = a + d;
	return(r);
}

pair
julia(x, y, p)		/* the Julia-Mandelbrot map using real rather than */
double x, y; 		/* complex coordinates */
double *p;
{
	pair coord;

	coord.x = (x*x) - (y*y) + p[0] + (p[2]*x);
	coord.y = (2.0*x*y) + p[1] + (p[3]*p[2]*y);
	return(coord);
}

pair
djulia(x, y, p)
double x, y;
double *p;
{
	pair r;

	/* calculate the determinant */
	r.x = (4.0*((x*x) + (y*y))) + (p[2]*((2.0*x*(p[3]+1)) + (p[2]*p[3])));
	/* calculate the trace */
	r.y = (4.0*x) + p[2] + (p[2]*p[3]);
	return(r);
}

pair
brussel(x, y, p)	/* The Brusselator flow after numerically integrating */
double x, y;		/* using the Euler method with time step p[0] */
double *p;
{
	pair coord;
	static double a, b;

	a = x*x*y;
	b = p[1]*x;
	coord.x = x + (p[0] * (p[2] - b + a - x));
	coord.y = y + (p[0] * (b - a));
	return(coord);
}

pair
dbrussel(x, y, p)
double x, y;
double *p;
{
	double a, b, c, d;
	pair r;

	d = 2.0*p[0]*x*y;
	a = 1.0 - (p[0]*p[1]) + d - p[0];
	b = p[0]*x*x;
	c = (p[0]*p[1]) - d;
	/* calculate the determinant */
	d = 1.0 - b;
	r.x = (a*d) - (b*c);
	/* calculate the trace */
	r.y = a + d;
	return(r);
}

pair
baru(x, y, p)	/* the Barugola map   "sur certaines zones chaotiques*/
double x, y; 
double *p;
{
	pair coord;
	static double A;
	extern double exp();

	A = ((p[3] * p[0]) / (1.0 - exp(p[2] * p[0])));
	coord.x = (x * exp(p[0] * (1.0 - p[1] * x) - (A * y)));
	coord.y = (x * ( 1.0 - exp(-A * y)));
	return(coord);
}

pair
dbaru(x, y, p)
double x, y;
double *p;
{
	static pair r;
	static double A;
	extern double exp();

	A = ((p[3] * p[0]) / (1.0 - exp(p[2] * p[0])));
	/* calculate the determinant */
	r.x = A*x*(1.0 - p[1] * p[0] * x) * exp((p[0] * (1.0 - p[1] * x)-(A * y)));
	/* calculate the trace later */
	r.y = trace(x, y, p);
	return(r);
}

#ifdef Plendo
pair
q1(x, y, p)	/* the Mira Quad map  */
double x, y; 
double *p;
{
	pair coord;
	static double xsq, ysq;

	xsq = x * x; ysq = y * y;
	coord.x = (p[0]*x) + (p[2]*y) + p[3] + (p[4]*xsq) + (p[5]*ysq) + p[6];
	coord.y = (p[7]*x) + (p[8]*y) + p[1] + (p[9]*xsq) + (p[10]*ysq) + p[11];
	return(coord);
}

pair
dq1(x, y, p)
double x, y;
double *p;
{
	pair r; 
	double pfx, pgy;

	pfx = p[0] + (2.0*p[4]*x);
	pgy = p[8]+(2.0*p[10]*y);
	r.x = pfx * pgy - ((p[7] + (2.0*p[9]*x))*(p[2]+(2.0*p[5]*y)));
	r.y = pfx + pgy;
	return(r);
}

pair
q2(x, y, p)	/* the Mira Agg map  */
double x, y; 
double *p;
{
	pair coord;

	coord.x = (p[0]*x) + (p[2]*y);
	coord.y = (p[3]*x) + (p[5]*y) + + (p[4]*x*x) + (p[1]*x*y);
	return(coord);
}

pair
dq2(x, y, p)
double x, y;
double *p;
{
	pair r;

	r.x = p[0]*(p[5]+(p[1]*x));
	r.x -= p[2]*(p[3]+(p[1]*y)+(2.0*p[4]*x));
	r.y = p[0] + p[5] + (p[1]*x);
	return(r);
}

pair
q3(x, y, p)	/* the Mira-Narayaninsamy map  */
double x, y; 
double *p;
{
	pair coord;

	coord.x = p[0] + (p[1]*x) + (x*x) - (y*y);
	coord.y = (2.0*x*y) - (2.5*p[1]*y);
	return(coord);
}

pair
dq3(x, y, p)
double x, y;
double *p;
{
	pair r;

	r.x = 4.0*((x*x) + (y*y));
	r.x -= 3.0*p[1]*x;
	r.x -= 2.5*p[1]*p[1];
	r.y = (4.0*x) - (1.5*p[1]);
	return(r);
}

pair
c1(x, y, p)	/* the Kawakami-Kawasaki map  */
double x, y; 
double *p;
{
	pair coord;

	coord.x = (p[1]*x) + (p[2]*y);
	coord.y = (p[0]*x) + (p[3]*y) + (p[4]*x*x*x);
	return(coord);
}

pair
dc1(x, y, p)
double x, y;
double *p;
{
	pair r;

	r.x = p[1]*p[3];
	r.x -= p[2]*(p[0] + (3.0*p[4]*x*x));
	r.y = p[1] + p[3];
	return(r);
}
#endif Plendo

pair
dnumeric(x, y, p)
double x, y;
double *p;
{
	static pair c, cx, cy;
	static pair r;
	extern PFP map;
	extern double delta;

	c = (*map)(x, y, p);
	cx = (*map)(x+delta, y, p);
	cy = (*map)(x, y+delta, p);

	/* first, calculate the determinant of the Jacobian */
	r.x = (cx.x*cy.y)-(cx.x*c.y)-(c.x*cy.y)-(cy.x*cx.y)+(cy.x*c.y)+(c.x*cx.y);
	r.x /= (delta*delta);
	/* next, calculate its trace */
	r.y = cx.x + cy.y - c.x - c.y;
	r.y /= delta;
	return(r);
}
