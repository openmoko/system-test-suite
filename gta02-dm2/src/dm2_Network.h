/* dm2_Network.h
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

#ifndef dm2_Network_H_
#define dm2_Network_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * define Global Value.
 */
enum{
 FALSE=0,
 TRUE		
};

#define DEFAULTIP "192.168.0.1"

#define DEFAULTPORT "80"

#define DEFAULTBACK "10"

#define MAXBUF        1024

#define MAXINTERFACES   16

//#define DEFAULTDIR "/mnt/ram"
#define DEFAULTDIR "tmp/"//"./"//"tmp/"

#define MAXPATH        150

void prterrmsg(char *msg);
void wrterrmsg(char *msg);
void prtinfomsg(char *msg);
void wrtinfomsg(char *msg);

int Start_Server(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __dm2_Network_H__ */
