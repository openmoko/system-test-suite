#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>

#include "dm_init.h"

struct nmea_gsv {
	char sentence[8];
	char nose[4];
	char sn[4];
	char nosa[4];
	char sate[4][4][4];
	char checksum[4];
	int  n;
};

#ifndef BUFSIZ
#define BUFSIZ 128*4
#endif

#define GPS_DEVICE			"/dev/ttySAC1"
#define GPS_TEST_TIME	60*3

#define GGA "$GPGGA"
#define RMC "$GPRMC"
#define GSV "$GPGSV"
#define GSA "$GPGSA"
#define GLL "$GPGLL"
#define ZDA "$GPZDA"
#define VTG "$GPVTG"
#define LOR "$PGLOR"

int GPGGA(char* str, struct nmea_gga* gga);
int GPGSV(char* str, struct nmea_gsv* gsv);
int GPGSA(char* str, struct nmea_gsa* gsa);

static char delim[] = ",*";
static char line_buffer[1024] = { 0 };
static struct nmea_gsv n_gsv[3];
int pass = 0;
static int poweron_process(void)

{
	int res,vol=0,i=0,j=0,mul;
	char *reqbuf=NULL,*tp=NULL,vol2;
	//char reqbuf[128];
	FILE *fp=NULL;
// /sys/bus/i2c/drivers/pcf50633/0-0073/neo1973-pm-gps.0/ 
	fp	= fopen("/sys/devices/platform/neo1973-pm-gps.0/power_on", "w");	
	if (NULL == fp)
    	{
        return printf("[GPS] Can't Open GPS sysfs power control\r\n");
    	}
	res = fwrite("1",sizeof(char),1, fp);
	fclose(fp);

	fp	= fopen("/sys/devices/platform/neo1973-pm-gps.0/power_on", "r");	
	reqbuf = malloc(BUFSIZ);
	memset(reqbuf,0,BUFSIZ);	
	res = fread(reqbuf, sizeof(char),BUFSIZ, fp);		
	//printf("res =%d ,%s\n",res,reqbuf);
	if (res != 0)
	{
		while (i < res)
		{
			if (reqbuf[i] == '=')
			{
				i++;				
				tp = reqbuf+i;
				break;
			}
			i++;
		}
	}
	i = res - i -1;
	for (;i>0;i--)
	{	
		mul = 1;			
		for (j=i;j>1;j--)
			mul = mul*10; 			
		vol2 = *tp -0x30;		
		vol = vol+ ( vol2 * mul);
		tp++;	
	} 
	//printf("vol=%d\n",vol);
	if (reqbuf[0] == '1' && vol == 3000)
	{
		printf("[GPS] Power on ok ; PMU %s",&reqbuf[3]);
		free(reqbuf);
		return 1;
	}	
	else
	{	
		printf("[GPS] Power on failed ; PMU %s",&reqbuf[3]);
		free(reqbuf);	
		return 0;	
	}
}

static void ttydevice_setting(char on)
{
	int fd,ret;
	struct termios ti;
	
	fd=open(GPS_DEVICE, O_RDWR);
	
	ret = tcgetattr(fd, &ti);
	if (ret<0)
		return;

	ret = cfsetispeed(&ti, B0);
	if (ret < 0)
		return;

	ret = cfsetospeed(&ti, B9600);
	if (ret < 0)
		return;

	//ti.c_cflag &= ~CRTSCTS;
	ti.c_lflag &= ~ECHO;
	if (!on)
	{
		ti.c_lflag |= ICANON;
		ti.c_cc[VMIN] = 0;
		ti.c_cc[VTIME] = 0;	
	}
	else
	{
		ti.c_lflag &= ~ICANON;
		ti.c_cc[VMIN] = 0;
		ti.c_cc[VTIME] = 10;	
	}
	tcsetattr(fd, TCSANOW, &ti);
	close(fd);
}

static void agps_nmea_read(char* buf)
{
    char* p;

	if ((strlen(buf) + strlen(line_buffer)) < sizeof(line_buffer) - 1)
    	{
        strcat(line_buffer, buf);
    	}
   else
    	{
        printf("Discarding \"%s\"\n", buf);
    	}

   do
    	{
		char tmp[1024];
		unsigned int s;

		p = strchr(line_buffer, '\n');
		if (!p)
			return;

		p++;
		s = p - line_buffer;

		if (s < sizeof(tmp) - 1)
        	{
	    	if (s > 1)
	    		{
				memcpy(tmp, line_buffer, s);
				tmp[s] = '\0';

				//agps_nmea_process_(label, tmp);
	    		}
        	}
      else
        	{
	    	line_buffer[s - 1] = '\0';
         printf("Discarding long \"%s\"\n", line_buffer);
        	}

		s = strlen(p);
		if (s > 0)
			memmove(line_buffer, p, s);
		line_buffer[s] = '\0';
    	}
	while (p);
}
static int gps_nmea_verify(int fd,char* buf)
{
	char *p,*s;
	int i,res, j=0, k=0;	
	char tmp[512];
	char tmp3[32];
	memset(line_buffer,0,1024);
	while(1)
	{
		memset(line_buffer, 0, sizeof(line_buffer));
		res = read(fd,buf,BUFSIZ);
		strcpy(line_buffer, buf);
		if ((strlen(buf) + strlen(line_buffer)) < sizeof(line_buffer) - 1)
    		{
      	strcat(line_buffer, buf);
    		}
   	else
    		{
			printf("Discarding \"%s\"\n", line_buffer);
			return 0;
    		}

		p = strchr(line_buffer, '\n');
		s = strchr(line_buffer, '$');	
		if (p!=NULL && s!=NULL)
		{
			if (strstr(line_buffer,GGA)||strstr(line_buffer,RMC)||\
				 strstr(line_buffer,GSA)||strstr(line_buffer,GSV)||\
				 strstr(line_buffer,GLL)||strstr(line_buffer,ZDA)||\
				 strstr(line_buffer,VTG))
				return 1;			
			else
				return 0;
		}
	}
}

static int gps_nmea_check(int fd,char* buf)
{
	int i,res;	
	struct nmea_gsv* gsv = n_gsv;

	memset(buf,0,BUFSIZ);		
	res = read(fd,buf,BUFSIZ);

	if(buf[0] == NULL)
	{
		printf("[GPS] NMEA data receive failed\n");
		return; 
	}

	if (strstr(buf,GSV))
	{		
		memset((gsv), 0, sizeof(struct nmea_gsv));
		i = GPGSV(buf, (gsv));
		if(i) {
			printf("[GPS] Signal testing OK\n");
			pass = 1;
		}
	}
}

void gps_test(void)
{
	int res,fd,i;	
	char *buf;
	char coldstart_stop[13]= {0xb5,0x62,0x06,0x04,0x04,0x00,0xff,0x07,0x08,0x00,0x1c,0x85,0x00};
	char coldstart_start[13]={0xb5,0x62,0x06,0x04,0x04,0x00,0xff,0x07,0x09,0x00,0x1d,0x87,0x00};
	
	//GPS Power testing	
	if (!poweron_process())
	{
		return;	
	}

	//Uart receiving testing
	ttydevice_setting(1);
	buf = malloc(BUFSIZ);
	memset(buf,0,BUFSIZ);
	fd=open(GPS_DEVICE, O_RDWR);			
	res = read(fd,buf,BUFSIZ);
	if(buf[0] == NULL)
	{
		printf("[GPS] NMEA data receive failed 00\n");
		return; 
	}
	ttydevice_setting(0);
	printf("[GPS] NMEA data receive OK\n");
	//Cold start testing
	ttydevice_setting(1);
	res = write(fd,coldstart_stop,12);	
	sleep(1);	
	res = read(fd,buf,BUFSIZ);
	memset(buf,0,BUFSIZ);
	res = read(fd,buf,BUFSIZ);
	//printf("read res =%d",res);
	if(buf[0] != NULL)
	{
		printf("[GPS] Cold start testing failed 00\n");
		return; 
	}
	res = write(fd,coldstart_start,12);	
	sleep(1);
	ttydevice_setting(0);
	if(!gps_nmea_verify(fd,buf))
	{
		printf("[GPS] Cold start testing failed 01\n");
		return; 
	}
	printf("[GPS] Cold start testing OK\n");
	free(buf);	

	for(i=0;i<GPS_TEST_TIME;i++)
	{	 
		buf = malloc(BUFSIZ);
		if(!(gps_nmea_check(fd,buf)))
		{
			return;
		}
		free(buf);
		if(pass) i=GPS_TEST_TIME;
	}

	close(fd);
}

int GPGSV(char* str, struct nmea_gsv* gsv)
{
    char* ptr = str;
    char* pptr;
    int i = 0;
    int j = 0;
    int k = 0;
    int done = 0;
    int checksum = 0;

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

                printf("PRN= %d, CN: %d\n", prn, CN0);

					if(CN0 >= 45) return 1;

                k = 0;                  
                ++j;                       
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


