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

pair GNP, RK;
double pB, A2T, CT, A1T, ET, FT, GT; 

#ifdef NorthSouth

pair
north(x, y, p) /* North-South model from Chichilnisky, Abraham & Record */
double x, y; 
double *p;
{
	static pair coord;
	extern void gamma_quadratic();
	extern void compute_gnp();

	gamma_quadratic(x, y, p);
	compute_gnp(x, y, p);
#ifdef RESTRICT
/*	Restrict to non-negative GNP */
 	coord.x = Max(0.0,(p[13] * GNP.x) + ((1.0 - p[12]) * x));
 	coord.y = Max(0.0,(p[1] * GNP.y) + ((1.0 - p[0]) * y));
#else
/* this does not restrict to non-negative gnp  */
	coord.x = (p[13] * GNP.x) + ((1.0 - p[12]) * x);
 	coord.y = (p[1] * GNP.y) + ((1.0 - p[0]) * y);
#endif
	return(coord);
}

pair
dnorth(x, y, p)
double x, y;
double *p;
{
	extern void gamma_quadratic(); 
	extern pair dmodels();

	gamma_quadratic(x, y, p);
	return(dmodels(x, y, p));
}

pair
dmodels(x, y, p)
double x, y;
double *p;
{
	static double r, dpBdx, dpBdy, dxdx, dxdy, dydx, dydy, dnsq, dssq;
	static double a2n, dn, c1n, lbarn, alphan, c2n, sn, deltan, cnsq, cssq;
	static double a2s, ds, c1s, lbars, alphas, c2s, ss, deltas, a1n, a1s;
	static pair z;

printf("");
	z.x = z.y = MAXDOUBLE;
	a1n = p[14]; a2n=p[15]; dn=p[18]; c1n=p[16]; lbarn=p[20]; 
	alphan=p[21]; c2n=p[17]; sn=p[13]; deltan=p[12];
	a1n = p[2]; a2s=p[3]; ds=p[6]; c1s=p[4]; lbars=p[8]; 
	alphas=p[9]; c2s=p[5]; ss=p[1]; deltas=p[0];
	dnsq = dn * dn; dssq = ds * ds;
	cnsq = c1n * c1n; cssq = c1s * c1s;
	if ((pB == 0.0)||(dn == 0.0)||(ds == 0.0)||(dnsq == 0.0)||(dssq == 0.0))
		return(z);
	r =  (1.0/dn) * ((c1n * lbarn) + (alphan * c1n * c2n / dn));
	r += (1.0/ds) * ((c1s * lbars) + (alphas * cssq / ds));
	r /= (a2n * x / dn) + (a2s * y / ds);
	dpBdx = r * a2n / (2.0 * dn);
	dpBdy = r * a2s / (2.0 * ds);
	r = (alphan*c2n*c2n/dnsq) + (c2n*lbarn/dn) - (a2n*x/dn);
	dxdx=(-1.0*pB*a2n/dn)+(r*dpBdx)+(a1n/dn)-((alphan*cnsq)/(dnsq*pB*pB));
	dxdx *= sn;
	dxdx += 1.0 - deltan;
	dxdy = sn * ((r - (alphan*cnsq/(dnsq*pB*pB))) * dpBdy);
	r = (alphas*c2s*c2s/dssq) + (c2s*lbars/ds) - (a2s*y/ds);
	dydy=(-1.0*pB*a2s/ds)+(r*dpBdy)+(a1s/ds)-((alphas*cssq)/(dssq*pB*pB));
	dydy *= ss;
	dydy += 1.0 - deltas;
	dydx = ss * ((r - (alphas*cssq/(dssq*pB*pB))) * dpBdx);
	z.x = (dxdx * dydy) - (dxdy * dydx);
	z.y = dxdx + dydy;
	return(z);
}

void
solve_quadratic(x, y, p)
double x, y;
double *p;
{
	static double pB1, pB2, a, b, c, q;
	extern double sqrt();
	extern double   VT;

	a = (x*p[15]/p[18]) + (y*p[3]/p[6]);
	b = VT * (-1.0); 
	c = (p[9]*p[4]*p[4]/(p[6]*p[6])) + (p[21]*p[16]*p[16]/(p[18]*p[18]));
	if (b < 0)
		q = -0.5 * (b - (sqrt((b*b) - (4.0*a*c))));
	else
		q = -0.5 * (b + (sqrt((b*b) - (4.0*a*c))));
	if (a == 0.0)
		pB1 = 0.0;
	else
		pB1 = q/a;
	if (q == 0.0)
		pB2 = 0.0;
	else
		pB2 = c/q;
	if ((pB = Min(pB1, pB2)) <= 0)
		pB = Max(pB1, pB2);
}

void
gamma_quadratic(x, y, p)
double x, y;
double *p;
{
	static double pB1, pB2, a, b, c, q, De_sq, n_De_sq;
	extern double sqrt();
	extern double   WT;

	n_De_sq = p[18] * p[18]; De_sq = p[6] * p[6];
	a = (p[21]*p[17]*p[17]/(n_De_sq))-(((x*p[15])+(p[17]*p[20]))/p[18]);
	b = a * (1.0 - p[23]);
	c = (p[9]*p[5]*p[5]/(De_sq))-(((y*p[3])+(p[5]*p[8]))/p[6]);
	q = c * (1.0 - p[11]);
	ET = b + q;
	CT = ((1.0/p[18])*((p[16]*p[20])+(p[21]*p[16]*p[17]/p[18])-(p[14]*x)))+
		 ((1.0/p[6])*((p[4]*p[8]) + (p[9]*p[4]*p[5]/p[6]) - (p[2]*y)));
	a = 2.0*p[21]*p[16]*p[17]/n_De_sq;
	b = x*p[14]/p[18];
	c = p[16]*p[20]/p[18];
	q = 1.0 - p[23];
	GT = q * (b - a - c);
	a = 2.0*p[9]*p[4]*p[5]/De_sq;
	b = y*p[2]/p[6];
	c = p[4]*p[8]/p[6];
	q = 1.0 - p[11];
	FT = GT + (q * (b - a - c));
	a = (1.0 - p[23]) * (p[21]*p[16]*p[16]/n_De_sq);
	b = (1.0 - p[11]) * (p[9]*p[4]*p[4]/De_sq);
	GT = a + b;
	a = ET; b = CT + FT; c = GT - WT;
	if (b < 0)
		q = -0.5 * (b - (sqrt((b*b) - (4.0*a*c))));
	else
		q = -0.5 * (b + (sqrt((b*b) - (4.0*a*c))));
	if (a == 0.0)
		pB1 = 0.0;
	else
		pB1 = q/a;
	if (q == 0.0)
		pB2 = 0.0;
	else
		pB2 = c/q;
/*	if ((pB = Min(pB1, pB2)) <= 0)  */
		pB = Max(pB1, pB2);
}

void 
compute_rk(x, y, p)
double x, y;
double *p;
{
	if (p[18] != 0.0)
		RK.x = (p[14] - (p[15]*pB))/p[18];
	else {
		printf("Setting North RK to MAXFLOAT\n");
		RK.x = MAXFLOAT;
	}
	if (p[6] != 0.0)
		RK.x = (p[2] - (p[3]*pB))/p[6];
	else {
		printf("Setting South RK to MAXFLOAT\n");
		RK.y = MAXFLOAT;
	}
}

void 
compute_gnp(x, y, p)
double x, y;
double *p;
{
	static double n_L, s_L, Dsq;

	if (pB != 0.0) {
		Dsq = p[18] * p[18];
		n_L = (p[21]*p[17]*p[17]/Dsq) + (p[17]*p[20]/p[18]) - (p[15]*x/p[18]);
		s_L = (-2.0*p[21]*p[16]*p[17]/Dsq)+(p[14]*x/p[18])-(p[16]*p[20]/p[18]);
		GNP.x = (pB*n_L) + s_L + ((p[21]*p[16]*p[16])/(Dsq*pB));
		Dsq = p[6] * p[6];
		n_L = (p[9]*p[5]*p[5]/Dsq) + (p[5]*p[8]/p[6]) - (p[3]*y/p[6]);
		s_L = (-2.0*p[9]*p[4]*p[5]/Dsq)+(p[2]*y/p[6])-(p[4]*p[8]/p[6]);
		GNP.y = (pB*n_L) + s_L + ((p[9]*p[4]*p[4])/(Dsq*pB));
	}
	else {
		printf("Setting GNP's to MAXFLOAT\n");
		GNP.x = MAXFLOAT;
		GNP.y = MAXFLOAT;
	}
}
#endif /* NorthSouth */
