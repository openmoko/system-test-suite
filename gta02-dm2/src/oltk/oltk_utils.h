/* oltk_util.h
 *
 * (C) 2007 by OpenMoko, Inc.
 * Written by Jim Huang <jserv@openmoko.org>
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

#ifndef OLTK_INTERNAL_UTIL
#define OLTK_INTERNAL_UTIL

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3)) && defined(__ELF__)
#  define OLTK_EXPORT __attribute__((visibility("default")))
#else
#  define OLTK_EXPORT
#endif

#endif /* OLTK_INTERNAL_UTIL */

