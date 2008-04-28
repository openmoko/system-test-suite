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

static struct nmea_gga n_gga;
static struct nmea_rmc n_rmc;
static struct nmea_gsv n_gsv[GOT];
static struct nmea_gsa n_gsa;
static struct nmea_lor n_lor;

static unsigned int msStart;
static unsigned long fix_sec;

unsigned long msTime(void)
{
    struct tm* t;
    time_t aclock;  

    time(&aclock);           /* Get time in seconds */
    t = localtime(&aclock);  /* Convert time to struct tm form */

    unsigned long ms = t->tm_hour*3600 + t->tm_min*60 + t->tm_sec;    
    return ms;
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

#if 0
static void NMEA_MSG(const char* msg)
{
 printf("\n%s\n",msg);	
}

static void NMEA_DEBUG(const char* msg)
{

    NMEA_MSG(msg);

#ifdef SLOG			
    if (fp1) fputs(msg, fp1);
#endif			

}
#endif

int fix_good  = 0;
int fix_fail  = 0;
int fix_total = 0;

static const char*
nmea_epoch_end(struct nmea_gga* gga, struct nmea_lor* lor)
{
    char* tmp;    

    tmp = (char *)calloc(1,512);	

    char* p = tmp ;
	    	
    int nquallity;			    

    // TODO:  Report more info
    //        Be smarter about calling this procedure
    //        Save the data so we can press a "report" button to save to file.

    p += sprintf(p, "Start time: %02d:%02d:%02d\n", msStart/ 3600, (msStart % 3600) / 60, (msStart %   60));
  
    sscanf(gga->fix_quality,  "%d", &nquallity);
    p += sprintf(p, "Fixed indicates: %d\n", nquallity);
      	
    if(!nquallity)   
    {
        unsigned long total_sec = fix_sec =runtime();
        p += sprintf(p, "Elapsed time:");

        if (total_sec > 59)
        {
            unsigned int hours =  total_sec / 3600;
            unsigned int min   = (total_sec % 3600) / 60;
            unsigned int sec   = (total_sec %   60);

            p += sprintf(p, " %02d:%02d:%02d", hours, min, sec);

        }
        p += sprintf(p, " (%ld s)\n", total_sec);
    }
   else	
    {
        p += sprintf(p, "fixed time:");

        if (fix_sec > 59)
        {
            unsigned int hours =  fix_sec / 3600;
            unsigned int min   = (fix_sec % 3600) / 60;
            unsigned int sec   = (fix_sec %   60);

            p += sprintf(p, " %02d:%02d:%02d", hours, min, sec);

        }
        p += sprintf(p, " (%ld s)\n", fix_sec);
    }	
    
    if (gga->time[0])
    {
        char fixTime[32];

        fixTime[0] = gga->time[0];
        fixTime[1] = gga->time[1];
        fixTime[2] = ':';
        fixTime[3] = gga->time[2];
        fixTime[4] = gga->time[3];
        fixTime[5] = ':';
        fixTime[6] = gga->time[4];
        fixTime[7] = gga->time[5];
        fixTime[8] = gga->time[6];
        fixTime[9] = gga->time[7];
        fixTime[10] = gga->time[8];
        fixTime[11] = 0;
        p += sprintf(p, "       UTC: %s\n", fixTime);
    }
    else
    {
        p += sprintf(p, "       UTC: -\n");
    }

    if (strlen(gga->latitude) && strlen(gga->longitude))
    {
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
    }
    else
    {
        p += sprintf(p,
                     "  Latitude: -\n"
                     " Longitude: -\n"
                     "  Altitude: -\n"
                     "      HDOP: -\n");
    }
   
    if (gga->nos[0])
    {
        int nsat;
        sscanf(gga->nos,  "%d", &nsat);
        p += sprintf(p, "  Num sats: %d\n", nsat);
    }
    else
    {
        p += sprintf(p, "\n");
    }

    if (lor->rid[0])
    {
        p += sprintf(p, "    Version: %s\n", lor->rid);
    }
    if (lor->nIgr)
    {
        p += sprintf(p, "IGR: %d) %s", lor->nIgr, lor->igr);
    }
     free(tmp);	
//    printf("%s\n",tmp);
    return tmp;
}

const char* nmea_process(char*buf,int* fixed)
{
/*
    int i = 0;
    int j = 0;
    int k = 0;
    int nos = 0;

    struct nmea_rmc* rmc = &n_rmc;
    struct nmea_gsv* gsv = &n_gsv;
    struct nmea_gsa* gsa = &n_gsa; 
*/
    struct nmea_gga* gga = &n_gga;
    struct nmea_lor* lor = &n_lor;

    if (strstr(buf,GGA))
    {	
        memset(gga, 0, sizeof(struct nmea_gga));
        
        GPGGA(buf, gga);						

	sscanf(gga->fix_quality,"%d", fixed);
/*
	printf("Sentence: %s\n", gga->sentence);
       printf("FIX Taken: %s\n", gga->time);
       printf("Latitude: %s ", gga->latitude);
       printf("%s\n", gga->la);
       printf("Longitude: %s ", gga->longitude);
       printf("%s\n", gga->lo);
       printf("Fix quality: %s\n", gga->fix_quality);
       printf("Num of sat: %s\n", gga->nos);
       printf("Horizontal dilution: %s\n", gga->hdop);
       printf("Altitude: %s ", gga->alme);
       printf("%s\n", gga->al);
       printf("Height of geoid: %s ", gga->hog);
       printf("%s\n", gga->ho);
       printf("time in seconds: %s\n", gga->tis);
       printf("DGPS ID: %s\n", gga->DGPS);
       printf("checksum: %s\n", gga->checksum);
*/
      return nmea_epoch_end(gga, lor);       	
    } 
/*
    else if (strstr(buf,RMC))
    {
        memset(rmc, 0, sizeof(struct nmea_rmc));
       // NMEA_DEBUG("$GPRMC");
        if (strstr(buf, ",A,"))
        {
            ++fix_good;
        }
        else
        {
            ++fix_fail;
        }
        ++fix_total;
        GPRMC(buf, rmc);	
        printf("RMC: (%d+%d)/%d\n", fix_good, fix_fail, fix_total);
//	return NULL;	 
    }

    else if (strstr(buf,GSA))
    {
        memset(gsa, 0, sizeof(struct nmea_gsa));
        GPGSA(buf, gsa);

        printf("Sentence: %s\n", gsa->sentence);
        printf("Auto/Manual: %s\n", gsa->AM);
        printf("mode: %s\n", gsa->mode);

        for (i = 0; i < 12; ++i) 
        {
            printf("PRN(%d): %s\n", i, gsa->id[i]);
        }

        printf("PDOP: %s\n", gsa->PDOP);
        printf("HDOP: %s\n", gsa->HDOP);
        printf("VDOP: %s\n", gsa->VDOP);
        printf("checksum: %s\n", gsa->checksum);
        printf("Num of Sate: %d\n",gsa->n);

     //   return NULL;
    }
   else if (strstr(buf,GSV))
    {
        struct nmea_gsv* pGSV;

        memset((gsv+j), 0, sizeof(struct nmea_gsv));

        GPGSV(buf, (gsv+j));	

        pGSV = gsv+j;
        printf("Sentence: %s\n",        pGSV->sentence);
        printf("Num of sentence: %s\n", pGSV->nose);
        printf("sentence #: %s\n",      pGSV->sn);
        printf("Num of sate: %s\n",     pGSV->nosa);

        for (i = 0; i < pGSV->n; ++i)
        {
            printf("(%d) PRN: %s\n",       i, pGSV->sate[i][0]);
            printf("(%d) Elevation: %s\n", i, pGSV->sate[i][1]);
            printf("(%d) Azimuth: %s\n",   i, pGSV->sate[i][2]);
            printf("(%d) SNR: %s\n",       i, pGSV->sate[i][3]);
        }

        printf("checksum: %s\n", (gsv+j)->checksum);

        ++j;
//	return NULL;
    }
    else if (strstr(buf,"PGLOR,RID"))
    {
        memset(lor, 0, sizeof(struct nmea_lor));
        PGLOR_RID(buf, lor);
	return NULL;
    }
    else if (strstr(buf,"PGLOR,IG"))
    {
        PGLOR_IGR(buf, lor);
	return NULL;
    }
    else if (strstr(buf,"PGLOR,FIX"))
    {
        PGLOR_FIX(buf, lor);        
	return NULL;
    }
*/
    else 
    {
        printf("unknown nmea %s\n", buf);
	return NULL;
    }
} 
//-------------------------------------------------------------------------------------
//
//      agps_nmea_process_()
//
//      Process the NMEA sentences.
//
//-------------------------------------------------------------------------------------

const char* agps_nmea_process(char* buf, int* fixed)
{
    static char line_buffer[320] = { 0 };
    static char *result;

    if(!strlen(buf))
 	return NULL;

    //printf("Get : %s",buf); 	
    if((result= strstr(buf,"$GPGGA"))==NULL)
    {
     printf("Not Found");
    }
    else
    {
     memcpy(line_buffer,result,sizeof(struct nmea_gga));
     printf("Found: %s\n",line_buffer);
    }
	
     if(strlen(line_buffer))
      {
	return nmea_process(line_buffer,fixed);
      }	
     else
      {	
	fixed=0;
	return NULL;
      }	
}	

extern int table_prn_cn[];
extern int already_beep;

const char* nmea_process2(char*buf,int* fixed)
{
    int i = 0;
    int j = 0;
    int k = 0;
    int nos = 0;
    *fixed = 0;

    struct nmea_rmc* rmc = &n_rmc;
    struct nmea_gsv* gsv = n_gsv;
    struct nmea_gsa* gsa = &n_gsa;

    struct nmea_gga* gga = &n_gga;
    struct nmea_lor* lor = &n_lor;

   if (strstr(buf,GSV))
    {
        struct nmea_gsv* pGSV = gsv + j;
        char buf2[64] = { 0 };
        char *out_text = calloc(1, 512);
        int hit_count = 0;
        memset(out_text, 0, 512);

        memset(pGSV, 0, sizeof(struct nmea_gsv));

	GPGSV(buf, pGSV);

        for (i = 0; i < 32; i++) {
                if (table_prn_cn[i] > 35) {
                        if (!already_beep ) {
				system("alsactl -f /etc/play_wav_speaker.state restore");
            		        system("aplay /usr/share/dm2/bru93q_7s.wav");
                                already_beep = 1;
                        }
                        sprintf(buf2, "prn: %d, C/N: %d\n", i, table_prn_cn[i]);
                        strcat(out_text, buf2);
                        hit_count++;
                }
        }

        ++j;
        if (hit_count == 0)
                strcpy(out_text, "N/A");
        else if (hit_count > 1)
                strcpy(out_text, "Fail");
        else {
                strcat(out_text, "- Pass -");
                *fixed = 1;
        }

        snprintf(buf2, 12, "\n(time: %ld)", runtime());
        strncat(out_text, buf2, 12);
        return out_text;
    }
    else
    {
        printf("unknown nmea %s\n", buf);
        return NULL;
    }
}

const char* agps_nmea_process2(char* buf, int* fixed)
{
        static char line_buffer[320] = { 0 };
        static char *result;

        if(!strlen(buf))
                return NULL;

	//printf("Get : %s",buf); 	

        if((result= strstr(buf,"$GPGSV")) != NULL) {
                memcpy(line_buffer,result,sizeof(struct nmea_gsv));
                printf("Found: %s\n",line_buffer);
        }
	else{
     	 printf("Not Found");
    	}

        if(strlen(line_buffer)) {
                if(nmea_process2(line_buffer,fixed)!=NULL)      
                        return nmea_process2(line_buffer,fixed);
                else {          
                        return NULL;
                }
        }       
        else {  
                return NULL;
        }
}

