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
#include "x.h"

/* Store color pics in PPM format and monochrome in PGM */
void
save_to_file(pix, w, h)
Pixmap pix;
int w, h;
{
	FILE *outfile;
/*	FILE *pickfile; */
	unsigned char c;
	XImage *ximage;
	static int i,j;
	struct Colormap {
		unsigned char red;
		unsigned char green;
		unsigned char blue;
	};
	struct Colormap *colormap=NULL;

	if (colormap)
		free(colormap);
	if ((colormap=
		(struct Colormap *)malloc(sizeof(struct Colormap)*numcolors))==NULL){
			fprintf(stderr,"Error malloc'ing colormap array.\n");
			Cleanup();
			exit(-1);
		}
	outfile = fopen(outname,"w");
/*	pickfile = fopen("pick.out", "w"); */
	if(!outfile) {
		perror(outname);
		Cleanup();
		exit(-1);
	}

	ximage=XGetImage(dpy, pix, 0, 0, w, h, AllPlanes, ZPixmap);

	if (displayplanes > 1) {
		for (i=0;i<numcolors;i++) {
			colormap[i].red=(unsigned char)(Colors[i].red >> 8);
			colormap[i].green=(unsigned char)(Colors[i].green >> 8);
			colormap[i].blue =(unsigned char)(Colors[i].blue >> 8);
		}
		fprintf(outfile,"P%d %d %d\n",6,w,h);
	}
	else
		fprintf(outfile,"P%d %d %d\n",5,w,h);
	fprintf(outfile,"# settle=%d  dwell=%d delta=%lf cdelta=%lf\n",
			settle, dwell, delta, cdelt);
	fprintf(outfile,"# min_x=%lf  a_rng=%lf  max_x=%lf\n",min_x,x_range,max_x);
	fprintf(outfile,"# min_y=%lf  b_rng=%lf  max_y=%lf\n",min_y,y_range,max_y);
	for (i=0;i<MAXPARAMS/2;i++)
		fprintf(outfile,"# params[%d]=%lf params[%d]=%lf\n",
				i,params[i],2*i,params[2*i]);
	fprintf(outfile,"# Map name = %s mapindex=%d\n",
			Mapnames[mapindex], mapindex);
	fprintf(outfile,"# find=%d portrait=%d critical=%d attractors=%d\n",
			find, portrait, critical, attractors);
	fprintf(outfile,"# lyap=%d p1=%d p2=%d start_x=%lf start_y=%lf\n",
			lyap, p1, p2, start_x, start_y);
	fprintf(outfile,"%d\n",numcolors-1);

	for (j=0;j<h;j++)
		for (i=0;i<w;i++) {
			c = (unsigned char)XGetPixel(ximage,i,j);
			if (displayplanes > 1)
				fwrite((char *)&colormap[c],sizeof colormap[0],1,outfile);
			else
				fwrite((char *)&c,sizeof c,1,outfile);
/*				fwrite((char *)&c,sizeof c,1,pickfile); */
		}
	fclose(outfile);
/*	fclose(pickfile); */
}
