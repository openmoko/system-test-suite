/* gps.h
 *
 * (C) 2007 by OpenMoko, Inc.
 * Written by Nod Huang <nod_huang@openmoko.com>
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef GPS_H_
#define GPS_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define TRUE (1)
#define FALSE (0)

void runtime_init();
const char* agps_nmea_process(char *buffer512, char* buf, int* fixed);
const char* agps_nmea_process2(char *buffer512, char* buf, int* fixed);
const char* nmea_process2(char *buffer512, char *buf, int *fixed);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GPS_H__ */
