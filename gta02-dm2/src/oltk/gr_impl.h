/* gr_impl.h
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

#ifndef _GR_IMPL_H
#define _GR_IMPL_H

#include "gr.h"

struct gr_backend {
	const char *name;
	struct gr *(*open)(const char *, int width, int height, int nonblock);
};

extern struct gr_backend gr_x11_backend;
extern struct gr_backend gr_fb_backend;

#endif /* _GR_IMPL_H */
