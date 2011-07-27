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


void
go_down() 
{
	frame++;
	if (frame > maxframe)
		frame = 0;
	jumpwin();
}

void
go_back() 
{
	frame--;
	if (frame < 0)
		frame = maxframe;
	jumpwin();
}

jumpwin()
{
	extern image_data_t rubber_data;

	rubber_data.p_min = min_x = a_minimums[frame];
	rubber_data.q_min = min_y = b_minimums[frame];
	rubber_data.p_max = max_x = a_maximums[frame];
	rubber_data.q_max = max_y = b_maximums[frame];
	x_range = max_x - min_x;
	y_range = max_y - min_y;
	x_inc = x_range / (double)width;
	y_inc = y_range / (double)height;
	point.x = 0;
	point.y = 0;
	maxexp = minexp = 0;
	run = 1;
	x = min_x;
	y = min_y;
	Clear(trajec);
	Clear(crijec);
	Clear(which);
	if (resized[frame])
		Redraw();
	else
		redraw(indices[frame], perind[frame], 0);
}

void
go_init() 
{
	frame = 0;
	jumpwin();
}

Destroy_frame()
{
	static int i;

	for (i=frame; i<maxframe; i++) {
		periods[frame] = periods[frame+1];
		indices[frame] = indices[frame+1];
		perind[frame] = perind[frame+1];
		a_minimums[frame] = a_minimums[frame+1];
		b_minimums[frame] = b_minimums[frame+1];
		a_maximums[frame] = a_maximums[frame+1];
		b_maximums[frame] = b_maximums[frame+1];
	}
	if (maxframe)
		maxframe--;
	go_back();
}

