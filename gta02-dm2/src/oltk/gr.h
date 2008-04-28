/* gr.h
 *
 * (C) 2007 by OpenMoko, Inc.
 * Written by Chia-I Wu <olv@openmoko.com>
 * All Rights Reserved
 *
 * This file is part of oltk.
 *
 * oltk is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * oltk is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with oltk; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 
 * 02110-1301 USA
 */ 

#ifndef _GR_H
#define _GR_H

struct gr_timeval {
	int tv_sec;
	int tv_usec;
};

struct gr_sample {
	int x;
	int y;
	unsigned int pressure;
	struct gr_timeval tv;
};

struct gr_rectangle {
	int x;
	int y;
	int width;
	int height;
};

struct gr_rgb {
	int red;
	int green;
	int blue;
};

struct gr {
	int width;
	int height;
	int pitch;

	void *fb;
	int bytes_per_pixel;
	int depth;

	int fd;

	void (*close)(struct gr *);
	void (*update)(struct gr*, struct gr_rectangle *, int);

	void (*set_color)(struct gr *, unsigned int, struct gr_rgb *, unsigned int);
	int (*get_color)(struct gr *, unsigned int);

	int (*sample)(struct gr* gr, struct gr_sample *);
};

struct gr *gr_open(const char *, int nonblock);

#endif /* _GR_H */
