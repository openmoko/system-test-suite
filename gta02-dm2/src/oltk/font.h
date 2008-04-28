/*
 *  font.h
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

#ifndef _FONT_H
#define _FONT_H

struct oltk_font_desc {
    const char *name;
    int width, height;
    const unsigned char *data;
    int pitch;
};

extern const struct oltk_font_desc oltk_font_fixed_8x13;

#endif /* _FONT_H */
