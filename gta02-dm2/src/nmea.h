/* nmea.h
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

#ifndef NMEA_H_
#define NMEA_H_

#define GGA_SENTENCE_ID "$GPGGA"
#define RMC_SENTENCE_ID "$GPRMC"
#define GSV_SENTENCE_ID "$GPGSV"
#define GSA_SENTENCE_ID "$GPGSA"
#define LOR_SENTENCE_ID "$PGLOR"
#define ZDA_SENTENCE_ID "$GPZDA"

struct nmea_gga {
	char sentence[8];
	char time[16];
	char latitude[16];
	char la[4];
	char longitude[16];
	char lo[4];
	char fix_quality[4];
	char nos[4];
	char hdop[8];
	char alme[8];
	char al[4];
	char hog[8];
	char ho[4];
	char tis[16];
	char DGPS[16];
	char checksum[4];
};

struct nmea_rmc {
	char sentence[8];
};

struct nmea_gsv {
	char sentence[8];
	char nose[4];
	char sn[4];
	char nosa[4];
	char sate[4][4][4];
	char checksum[4];
	int  n;
};

struct nmea_gsa {
	char sentence[8];
	char AM[4];
	char mode[4];
	char id[12][4];
	char PDOP[8];
	char HDOP[8];
	char VDOP[8];
	char checksum[4];
	int  n;
};

struct nmea_lor {
	int  nIgr;
	char sentence[8];
	char rid[32];
	char igr[80*5];
};

struct nmea_zda {
	int valid;
	unsigned long seconds;
	int day;
	int month;
	int year;
	int tz_adj_hours;
	int tz_adj_mins;
};


int GPGGA(char* str, struct nmea_gga* gga);
int GPGSV(char* str, struct nmea_gsv* gsv);
int GPGSA(char* str, struct nmea_gsa* gsa);
int GPRMC(char* str, struct nmea_rmc* rmc);
int PGLOR_IGR(char* str, struct nmea_lor* lor);
int PGLOR_RID(char* str, struct nmea_lor* lor);
int PGLOR_FIX(char* str, struct nmea_lor* lor);
int nmea_to_ZDA(char* str, struct nmea_zda* zda);

#endif
