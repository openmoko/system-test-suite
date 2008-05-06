/* nmea.c
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "nmea.h"

static char delim[] = ",*";

int GPGGA(char* str, struct nmea_gga* gga)
{
    char* ptr = str;
    char* pptr;	
    int i = 0;	
    int done = 0;	

    // g_printf("GGA(\"%s\")\n", str);

    for (ptr = str; !done && ptr; ++ptr)
    {
        for (pptr = ptr; *ptr!=delim[0] && *ptr!=delim[1]; ++ptr)
        {
            if (!*ptr)
            {
                done = 1;
                break;
            }		
        }
        *ptr = '\0';

        switch(i)
        {
        default: break;
        case 0: strncpy(gga->sentence,    pptr,  8); ++i; break;
        case 1: strncpy(gga->time,        pptr, 16); ++i; break;
        case 2: strncpy(gga->latitude,    pptr, 16); ++i; break;
        case 3: strncpy(gga->la,          pptr,  4); ++i; break;
        case 4: strncpy(gga->longitude,   pptr, 16); ++i; break;
        case 5: strncpy(gga->lo,          pptr,  4); ++i; break;
        case 6: strncpy(gga->fix_quality, pptr,  4); ++i; break;
        case 7: strncpy(gga->nos,         pptr,  4); ++i; break;
        case 8: strncpy(gga->hdop,        pptr,  8); ++i; break;
        case 9: strncpy(gga->alme,        pptr,  8); ++i; break;
        case 10: strncpy(gga->al,         pptr,  4); ++i; break;
        case 11: strncpy(gga->hog,        pptr,  8); ++i; break;
        case 12: strncpy(gga->ho,         pptr,  4); ++i; break;
        case 13: strncpy(gga->tis,        pptr, 16); ++i; break;
        case 14: strncpy(gga->DGPS,       pptr, 16); ++i; break;	
        case 15: strncpy(gga->checksum,   pptr,  4); ++i; break;	
        }
    }
    // g_printf("%d gga->time(\"%s\")\n", __LINE__, gga->time);
    return 0;
}

int table_prn_cn[32];

int GPGSV(char* str, struct nmea_gsv* gsv)
{
    char* ptr = str;
    char* pptr;
    int i = 0;
    int j = 0;
    int k = 0;
    int done = 0;
    int checksum = 0;

    // g_printf("\"%s\"", str);

    for (ptr = str; !done && ptr; ++ptr)
    {
        for (pptr = ptr; *ptr!=delim[0] && *ptr!=delim[1]; ++ptr)
        {
            if (!*ptr)
            {
                done = 1;
                break;
            }		
        }
        *ptr = '\0';

        if (*ptr == delim[1])
        {
            checksum = 1;             
        }

        switch(i)
        {
        default: break;
        case 0: strncpy(gsv->sentence, pptr, 8); ++i; break;      
        case 1: strncpy(gsv->nose,     pptr, 4); ++i; break;
        case 2: strncpy(gsv->sn,       pptr, 4); ++i; break;
        case 3:	strncpy(gsv->nosa,     pptr, 4); ++i; break;
        case 5: strncpy(gsv->checksum, pptr, 4); ++i; break;

        case 4: strncpy(gsv->sate[j][k], pptr, 4);
            if (++k >= 4)
            {
                int prn = atoi(gsv->sate[j][0]);
                int el  = atoi(gsv->sate[j][1]);
                int az  = atoi(gsv->sate[j][2]);
                int CN0 = atoi(gsv->sate[j][3]);

                // printf("%d) %s %s %s %s\n",
                //         j,
                //         gsv->sate[j][0], gsv->sate[j][1],
                //         gsv->sate[j][2], gsv->sate[j][3]);
		if (prn < 1)
			break;
		if (prn > 32)
			break;
		if (CN0 >= 3120)
			break;

                 printf("%d) prn %d  el %d  az %d  CN %d\n",
                             j, prn, el, az, CN0);
		 table_prn_cn[prn] = CN0;
               // page_SS_have_prn(prn, CN0, el, az);
		 

                k = 0;                  // Start over with the
                ++j;                    // ... next SV
            }			
            if (checksum)
            {
                gsv->n = j;                
                i = 5;
            }   
            break;					
        }
    }
    return 0;
}

int GPGSA(char* str, struct nmea_gsa* gsa)
{
    char* ptr;
    char* pptr;	
    int i = 0;	
    int j = 0;
    int done = 0;	
   // page_SS_unuse_prns();

    // g_printf("\"%s\"", str);

    for (ptr = str; !done && ptr; ++ptr)
    {
        for (pptr = ptr; *ptr!=delim[0] && *ptr!=delim[1]; ++ptr)
        {
            if (!*ptr)
            {
                done = 1;
                break;
            }		
        }
        *ptr = '\0';

        switch(i)
        {
        default: break;
        case 0: strncpy(gsa->sentence, pptr, 8); ++i; break;      
        case 1: strncpy(gsa->AM,       pptr, 4); ++i; break;
        case 2: strncpy(gsa->mode,     pptr, 4); ++i; break;
        case 4: strncpy(gsa->PDOP,     pptr, 8); ++i; break;
        case 5: strncpy(gsa->HDOP,     pptr, 8); ++i; break;
        case 6: strncpy(gsa->VDOP,     pptr, 8); ++i; break;			
        case 7: strncpy(gsa->checksum, pptr, 4); ++i; break;
        case 3:
            if (j < 12)
            {	
                strncpy(gsa->id[j], pptr, 4);
                j++;
            }
            else ++i;
            break;
        }
    }
    
    gsa->n = 0;
    for (i = 0; i < 12; ++i)
    {
        if (strlen(gsa->id[i]))
        {
            printf("page_SS_use_prn :%d",(atoi(gsa->id[i])));
            ++gsa->n;
        }
    }
//    page_SS_done();

    return 0;
}

int nmea_to_ZDA(char* str, struct nmea_zda* zda)
{
	float ds;
	if (sscanf(str, "$GPZDA,%f,%d,%d,%d,%d,%d*", &ds,
			&zda->day, &zda->month, &zda->year, &zda->tz_adj_hours,
			&zda->tz_adj_mins) != 6)
		return 1;
	zda->valid = 1;
	zda->seconds = (long)ds;
	return 0;
}


//void page_stats_fix_time(float ft);

int PGLOR_FIX(char* str, struct nmea_lor* lor)
{
    char* ptr = str;
    char* pptr;	
    int i = 0;	
    int done = 0;	

    for (ptr = str; !done && ptr; ++ptr)
    {
        for (pptr = ptr; *ptr!=delim[0] && *ptr!=delim[1]; ++ptr)
        {
            if (!*ptr)
            {
                done = 1;
                break;
            }		
        }
        *ptr = '\0';

        // printf("PGLOR/%d %d\n", __LINE__, i);
        switch(i)
        {
        default: ++i; break;
        case 2:                 // Get the fix time.
        {
            float ft;
            // printf("PGLOR/%d %s\n", __LINE__, pptr);
            if (1 == sscanf(pptr, "%f", &ft))
            {
                 printf("PGLOR/%d %f\n", __LINE__, ft);
                //page_stats_fix_time(ft);
            }
            ++i;
            break;
        }
        }
    }
    return 0;
}

//      Show integration problems with the platform
//      Skip the "$PGLOR,IGR," to collect the data up to "*"
//      Skip the "$PGLOR,IGC," to collect the data up to "*"

int PGLOR_IGR(char* str, struct nmea_lor* lor)
{
    char* c = 0;
    int   ch = ' ';
    if (strstr(str, ",IGR"))
    {
        ++lor->nIgr;
        lor->igr[0] = 0;
    }
    c = strstr(str, "*");
    if (c)
    {
        *c = 0;
    }
    str += 11;          // Skip the "$PGLOR,IGR,"

    //  Replace ',' in the IGR messages by ' ' alternating with '='.
    for (c = str; *c; ++c)
    {
        if (',' == *c)
        {
            *c = ch;
            if (' ' == ch) ch = '=';
            else           ch = ' ';
        }
        if ('.' == *c) *c = 'x';
    }

    strcat(lor->igr, str);
    strcat(lor->igr, "\n\r");
    // printf("PGLOR_IGR/%d \"%s\"\n", __LINE__, lor->igr);
    return 0;
}

//      Get the firmware version number
//      Skip the "$PGLOR,RID," to collect the data up to "*"

int PGLOR_RID(char* str, struct nmea_lor* lor)
{
    char* c = 0;
    c = strstr(str, "*");
    if (c) *c = 0;      // Truncate the checksum "*XX\n\r"

    str += 15;          // Skip the "$PGLOR,RID,GLL,"

    //  Replace ',' in the IGR messages by '.'
    for (c = str; *c; ++c)
    {
        if (',' == *c) *c = '.';
    }
    strcpy(lor->rid, str);
    printf("PGLOR_RID/%d %s\n", __LINE__, lor->rid);
    return 0;
}

// TODO:                Decode the RMC message.

int GPRMC(char* str, struct nmea_rmc* rmc)
{
    return 0;
}
