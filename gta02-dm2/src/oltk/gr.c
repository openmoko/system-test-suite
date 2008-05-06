/* gr.c
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

#include <stdio.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gr.h"
#include "gr_impl.h"

static const struct gr_backend *backends[] = {
#ifdef HAVE_X11
	&gr_x11_backend,
#endif
#ifdef HAVE_FB
	&gr_fb_backend,
#endif
};

static const int n_backends = sizeof(backends) / sizeof(backends[0]);

struct gr *gr_open(const char *dev, int nonblock)
{
	struct gr *gr;
	int i;

	for (i = 0; i < n_backends; i++)
	{
		gr = backends[i]->open(dev, 480, 640, nonblock);
		if (gr) {
			printf("gr_open: backend %d OK\n", i);
			break;
		} else {
			printf("gr_open: backend %d bad\n", i);
		}
	}

	return gr;
}
