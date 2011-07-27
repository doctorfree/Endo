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
#include "defines.h"

void
cmul(z, a, b, c, d)	/* complex multiplication */
double a, b, c, d;
pair *z;
{
	z->x = (a*c) - (b*d);
	z->y = (a*d) + (b*c);
}

/*
 * invert c; where:
 *    1           1                      a           b
 * --------  = ------- * (a - bi) = ( -------  -  ------- i )
 * (a + bi)      2   2                  2   2      2    2
 *              a + b                  a + b       a + b
 */
void
cinv(z, a, b)	/* complex inversion 1 / (a + bi) */
double a, b;
pair *z;
{
	static double factor;

	factor = 1.0/(a*a + b*b);
	z->x = a*factor;
	z->y = -b*factor;
}

void
cdiv(z, a, b, c, d)	/* complex division (a + bi) / (c + di) */
double a, b, c, d;
pair *z;
{
	cinv(z, c, d);
	cmul(z, a, b, z->x, z->y);
}

void
zbar(z, x, y)
double x, y;
pair *z;
{
	z->x = x;
	z->y = -y;
}

/*
 * void
 * zpow(z, x, y, n)	z^n for complex z = x + iy and integer n
 * double x, y;
 * pair *z;
 * int n;
 * {
 * 	static int i;
 * 
 * 	if (n == 0) {
 * 		z->x = 1; z->y = 0;
 * 		return;
 * 	}
 * 	z->x = x; z->y = y;
 * 	if (n < 0) {
 * 		n = -n;
 * 		cdiv(z, 1.0, 0.0, x, y);
 * 	}
 * 	for (i=1; i<n; i++)
 * 		cmul(z, z->x, z->y, x, y);
 * }
 */

void
zpow(z, x, y, n)	/* z^n for complex z = x + iy and real n */
double x, y, n;
pair *z;
{
	double logr, logi, a, b;
	double log(), exp(), cos(), sin(), atan2(), sqrt();

	logr = log( sqrt(x*x+y*y) );
	logi = atan2(y, x);

	a = exp( logr * n);
	b = logi * n;

	z->x = a * cos(b);
	z->y = a * sin(b);
}

double
rpzn(x, y, n)	/* real part of z^n for complex z = x + iy */
double x, y, n;
{
	pair z;

	zpow(&z, x, y, n);
	return(z.x);
}

void
cadd(z, a, b, c, d)	/* complex addition */
double a, b, c, d;
pair *z;
{
	z->x = a + c;
	z->y = b + d;
}

