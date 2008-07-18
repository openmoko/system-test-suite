/* gps.c
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

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

#include <sys/ioctl.h>

#include "oltk/oltk.h"
#include "nmea.h"
#include "gps.h"

#define GOT  3
#define CN_THRESHOLD   38

static struct nmea_gsv n_gsv[GOT];

static unsigned int msStart;
static unsigned long fix_sec;

unsigned long msTime(void)
{
    struct tm* t;
    time_t aclock;  

    time(&aclock);           /* Get time in seconds */
    t = localtime(&aclock);  /* Convert time to struct tm form */

    return t->tm_hour*3600 + t->tm_min*60 + t->tm_sec;
}

unsigned long int runtime_ms()
{	
    return msTime() - msStart;
}

unsigned long int runtime()
{
    return (runtime_ms());
}

void runtime_init()
{    
    msStart = msTime();    
}

int fix_good  = 0;
int fix_fail  = 0;
int fix_total = 0;

const char*
nmea_epoch_end(char * buf512, struct nmea_gga* gga, struct nmea_lor* lor,
		struct nmea_zda *zda, char * fixed_time)
{
    char *p = buf512;
    char *q = fixed_time;
    int nquallity = atoi(gga->fix_quality);
	static const char * months[] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep",
		"Oct", "Nov", "Dec"
	};

    // TODO:  Report more info
    //        Be smarter about calling this procedure
    //        Save the data so we can press a "report" button to save to file.

    p += sprintf(p, "Start time: %02d:%02d:%02d\n",
		     msStart / 3600,
		     (msStart % 3600) / 60,
		     (msStart % 60));
  
    p += sprintf(p, "Fixed indicates: %d\n", nquallity);
      	
    if(!nquallity) {
        unsigned long total_sec = fix_sec =runtime();
        p += sprintf(p, "Elapsed time:");

        if (total_sec > 59) {
            unsigned int hours =  total_sec / 3600;
            unsigned int min   = (total_sec % 3600) / 60;
            unsigned int sec   = (total_sec %   60);

            p += sprintf(p, " %02d:%02d:%02d", hours, min, sec);
        }
        p += sprintf(p, " (%ld s)\n", total_sec);
    } else {
        p += sprintf(p, "fixed time:");

        if (fix_sec > 59) {
            unsigned int hours =  fix_sec / 3600;
            unsigned int min   = (fix_sec % 3600) / 60;
            unsigned int sec   = (fix_sec %   60);

            p += sprintf(p, " %02d:%02d:%02d", hours, min, sec);

        }
        p += sprintf(p, " (%ld s)\n", fix_sec);
	q += sprintf(q, " (%ld s)\n", fix_sec);
    }	
    
    if (gga->time[0])
        p += sprintf(p, "       UTC: %c%c:%c%c:%c%c%c%c%c\n",
			gga->time[0],
			gga->time[1],
			gga->time[2],
			gga->time[3],
			gga->time[4],
			gga->time[5],
			gga->time[6],
			gga->time[7],
			gga->time[8]
	);
    else
        p += sprintf(p, "       UTC: -\n");

	if (zda->valid)
		p += sprintf(p, "       Date: %2d-%s-%04d\n",
				zda->day, months[zda->month], zda->year);
	else
		p += sprintf(p, "       Date:\n");

    if (gga->latitude[0] && gga->longitude[0]) {
        float lat;
        float lon;
        int alt;
        const char* alt_units = "m";

        sscanf(gga->latitude, "%f", &lat);
        lat /= 100.0;

        sscanf(gga->longitude, "%f", &lon);
        lon /= 100.0;

        sscanf(gga->alme, "%d", &alt);

        p += sprintf(p,
                     "  Latitude: %10.6f %s\n"
                     " Longitude: %10.6f %s\n"
                     "  Altitude: %d %s\n"
                     "      HDOP: %s\n",
             lat, gga->la,
             lon, gga->lo,
             alt, alt_units,
             gga->hdop);
    } else {
        p += sprintf(p,
                     "  Latitude: -\n"
                     " Longitude: -\n"
                     "  Altitude: -\n"
                     "      HDOP: -\n");
    }
   
    if (gga->nos[0]) {
        int nsat;
        sscanf(gga->nos,  "%d", &nsat);
        p += sprintf(p, "  Num sats: %d\n", nsat);
    } else
        p += sprintf(p, "\n");

    if (lor->rid[0])
        p += sprintf(p, "    Version: %s\n", lor->rid);

    if (lor->nIgr)
        p += sprintf(p, "IGR: %d) %s", lor->nIgr, lor->igr);

    return buf512;
}


extern int table_prn_sn[];
extern int already_beep;

const char* nmea_process2(char *buffer512, char *buf, int *fixed, char *cnr_buff)
{
	int i = 0;
        char *p = buffer512;
	char *q = cnr_buff;
        int hit_count = 0;
	char * pbufGSV = strstr(buf, GSV_SENTENCE_ID);

	*fixed = 0;
	
	if (!pbufGSV) {
		printf("unknown nmea %s\n", buf);
		return NULL;
	}

        memset(&n_gsv[0], 0, sizeof(struct nmea_gsv));
	GPGSV(pbufGSV, &n_gsv[0]);

        for (i = 0; i < 32; i++)
                if (table_prn_sn[i] >= CN_THRESHOLD) {
                        if (!already_beep ) {
				system("alsactl -f /etc/play_wav_speaker.state restore");
            		        system("aplay /usr/share/dm2/bru93q_7s.wav");
                                already_beep = 1;
                        }
                        p += sprintf(p, "prn: %d, S/N: %d\n", i, table_prn_sn[i]);
			q += sprintf(q, "prn: %d, S/N: %d\n", i, table_prn_sn[i]);
                        hit_count++;
                }

	switch (hit_count) {
	case 0:
		p += sprintf(p, "Fail\n");
		break;
	case 1:
                p += sprintf(p, "- Pass -\n");
                *fixed = 1;
		break;
        default:
                p += sprintf(p, "N/A\n");
		break;
	}

        sprintf(p, "(time: %ld)", runtime());

        return buffer512;
}

