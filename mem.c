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

int **histarray;
int maxhist = 0;
int minhist = MAXINT;

void
FreeHist()
{
	static int i;

	for (i=0; i<trawidth; i++)
		free(histarray[i]);
	free(histarray);
	maxhist = 0;
	minhist = MAXINT;
}

void
FreeCrit()
{
	if (crit_pts[0])
		free(crit_pts[0]);
	if (crit_pts[1])
		free(crit_pts[1]);
	if (crit_arc[0])
		free(crit_arc[0]);
	if (crit_arc[1])
		free(crit_arc[1]);
	if (n_crit_pts[0])
		free(n_crit_pts[0]);
	if (n_crit_pts[1])
		free(n_crit_pts[1]);
	if (n_crit_arc[0])
		free(n_crit_arc[0]);
	if (n_crit_arc[1])
		free(n_crit_arc[1]);
}

freemem()
{
	static int i;

	freecoords();
	for (i=0; i<MAXFRAMES; i++) {
		free(indices[i]);
		free(periods[i]);
		free(basins[i]);
	}
	if (histogram)
		FreeHist();
	if (critical)
		FreeCrit();
}

void
HistoMem()
{
	static int i, j;

	if ((histarray=
		(int **)malloc(sizeof(int)*trawidth))==NULL){
		fprintf(stderr,"Error malloc'ing x histarray array.\n");
		fprintf(stderr,"trawidth=%d\n",trawidth);
		exit(-1);
	}
	for (i=0;i<trawidth;i++)
		if ((histarray[i]=
			(int *)malloc(sizeof(int)*traheight))==NULL){
				fprintf(stderr,"Error malloc'ing x histarray array.\n");
				fprintf(stderr,"traheight=%d\n",traheight);
				exit(-1);
		}
	for (i=0;i<traheight;i++)
		for (j=0;j<trawidth;j++)
			histarray[j][i] = 0;
}

void
CritiMem()
{
	if ((crit_pts[0]=
		(double *)malloc(sizeof(double)*criwidth*criheight))==NULL){
		fprintf(stderr,"Error malloc'ing x crit_pts array.\n");
		fprintf(stderr,"criwidth=%d criheight=%d\n",criwidth,criheight);
		exit(-1);
	}
	if ((crit_pts[1]=
		(double *)malloc(sizeof(double)*criwidth*criheight))==NULL){
		fprintf(stderr,"Error malloc'ing y crit_pts array.\n");
		fprintf(stderr,"criwidth=%d criheight=%d\n",criwidth,criheight);
		exit(-1);
	}
	if ((crit_arc[0]=
		(double *)malloc(sizeof(double)*criwidth*criheight))==NULL){
		fprintf(stderr,"Error malloc'ing x crit_arc array.\n");
		fprintf(stderr,"criwidth=%d criheight=%d\n",criwidth,criheight);
		exit(-1);
	}
	if ((crit_arc[1]=
		(double *)malloc(sizeof(double)*criwidth*criheight))==NULL){
		fprintf(stderr,"Error malloc'ing y crit_arc array.\n");
		fprintf(stderr,"criwidth=%d criheight=%d\n",criwidth,criheight);
		exit(-1);
	}
	if ((n_crit_pts[0]=
		(double *)malloc(sizeof(double)*criwidth*criheight))==NULL){
		fprintf(stderr,"Error malloc'ing x n_crit_pts array.\n");
		fprintf(stderr,"criwidth=%d criheight=%d\n",criwidth,criheight);
		exit(-1);
	}
	if ((n_crit_pts[1]=
		(double *)malloc(sizeof(double)*criwidth*criheight))==NULL){
		fprintf(stderr,"Error malloc'ing y n_crit_pts array.\n");
		fprintf(stderr,"criwidth=%d criheight=%d\n",criwidth,criheight);
		exit(-1);
	}
	if ((n_crit_arc[0]=
		(double *)malloc(sizeof(double)*criwidth*criheight))==NULL){
		fprintf(stderr,"Error malloc'ing x n_crit_arc array.\n");
		fprintf(stderr,"criwidth=%d criheight=%d\n",criwidth,criheight);
		exit(-1);
	}
	if ((n_crit_arc[1]=
		(double *)malloc(sizeof(double)*criwidth*criheight))==NULL){
		fprintf(stderr,"Error malloc'ing y n_crit_arc array.\n");
		fprintf(stderr,"criwidth=%d criheight=%d\n",criwidth,criheight);
		exit(-1);
	}
}

setupmem()
{
	static int i;

	for (i=0;i<MAXFRAMES;i++) {
		if((indices[i]=
			(int *)malloc(sizeof(int)*width*height))==NULL){
			fprintf(stderr,"Error malloc'ing indices array.\n");
			fprintf(stderr,"width=%d height=%d\n",width,height);
			exit(-1);
		}
		if((periods[i]=
			(int *)malloc(sizeof(int)*width*height))==NULL){
			fprintf(stderr,"Error malloc'ing periods array.\n");
			fprintf(stderr,"width=%d height=%d\n",width,height);
			exit(-1);
		}
		if((basins[i]=
			(int *)malloc(sizeof(int)*width*height))==NULL){
			fprintf(stderr,"Error malloc'ing basins array.\n");
			fprintf(stderr,"width=%d height=%d\n",width,height);
			exit(-1);
		}
	}
	setupcoords();
	if (histogram)
		HistoMem();
	if (critical)
		CritiMem();
}

setupcoords()
{
	static int i;

	for (i=0;i<MAXFRAMES;i++) {
		if ((coords[i][0]=
			(double *)malloc(sizeof(double)*dwell))==NULL){
			fprintf(stderr,"Error malloc'ing x coords array.\n");
			fprintf(stderr,"dwell=%d\n",dwell);
			exit(-1);
		}
		if ((coords[i][1]=
			(double *)malloc(sizeof(double)*dwell))==NULL){
			fprintf(stderr,"Error malloc'ing y coords array.\n");
			fprintf(stderr,"dwell=%d\n",dwell);
			exit(-1);
		}
	}
}

freecoords()
{
	static int i;

	for (i=0; i<MAXFRAMES; i++) {
		free(coords[i][0]);
		free(coords[i][1]);
	}
}
