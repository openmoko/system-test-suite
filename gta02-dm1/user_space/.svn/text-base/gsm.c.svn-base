#include "dm_init.h"

#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif

#define ModemDevice "/dev/ttySAC0"
#define CRTSCTS   020000000000          /* flow control */

#ifndef BUFFSIZE
#define BUFFSIZE 256
#endif

/* descriptor for the terminal port */
static int gsm_channel = -1;

static int poweron_process(void)
{
	int res, fp, rc=0;;
	
	fp      = fopen("/sys/devices/platform/neo1973-pm-gsm.0/power_on", O_RDWR);
	if (fp == -1 ) {
                printf("Power on fail.\n");
                return errno;
        }
	rc = write(fp, "0", 1);
	sleep(1);
	rc = write(fp, "1", 1);

        if (rc == -1) {
                printf("Power on fail\n");
                close(fp);
                return errno;
        }

	close(fp);
	return 1;
}

static int modem_comm_init(void)
{
	struct termios t;

	gsm_channel = open(ModemDevice, O_RDWR | O_NOCTTY);
	if (gsm_channel == -1)	{
		perror("Open modem fail.\n");
		return errno;
	}

	if (tcgetattr(gsm_channel, &t) != 0) {
		perror("tcgetattr error \r\n");
		return errno;
	}

	/* set it to zero, read() will return immediately. */
	t.c_cc[VMIN] = 0;
	t.c_cc[VTIME] = 0;

	t.c_iflag &= ~(BRKINT 	/* Ignore break */
			| IGNPAR | PARMRK | /* Ignore parity */
			INPCK | /* Ignore parity */
			ISTRIP | /* Don't mask */
			INLCR | IGNCR | ICRNL /* No <cr> or <lf> */
			| IXON); /* Ignore STOP char */

	t.c_iflag |= IGNBRK | IXOFF; /* Ignore BREAK send XON and XOFF
					for flow control. */
	t.c_oflag &= ~(OPOST); /* No output flags */

#if 0
	/* try to close echo */
	   t.c_lflag = ICANON; /* no canonical input */
#endif
	t.c_lflag &= ~( /* No local flags. In */
			ECHO|ECHOE|ECHOK|ECHONL| /* particular, no echo */
			ICANON |	/* no canonical input */
			/* processing, */
			ISIG |      /* no signals, */
			NOFLSH | /* no queue flush, */
			TOSTOP); /* and no job control. */
	t.c_cflag &= ( /* Clear out old bits */
			CSIZE | /* Character size */
			CSTOPB |/* Two stop bits */
			HUPCL | /* Hangup on last close*/
			PARENB | /* Parity */
			CRTSCTS); /* HW flow control */
	t.c_cflag |=
			CLOCAL | /* no modem */
			CREAD | /* Enable receiver */
			CS8; /* 8-bit data */

	/* Assign I/O speeds */
	if (cfsetospeed(&t, B57600) == -1) {
		perror("[comm_init] cfsetospeed \r\n");
	}

	/* Throw away any input/output data (noise) */
	if (tcflush(gsm_channel, TCIOFLUSH) == -1)
		perror("[comm_init] tcflush \r\n");

	/* Set the termial port attributes */
	if (tcsetattr(gsm_channel, TCSANOW, &t) == -1)
		perror("[comm_init] tcflush \r\n");

	return 1;
}

static int gsm_listen(char *response)      
{
	char buf[BUFFSIZE];
	int count = 0;
	int j = 0;
	int success = -1;
	int readtimes = 0;
	char *p_strstr = NULL;

	while (readtimes++ < 5) { /* Loop 5 times */
		count = read(gsm_channel, buf, BUFFSIZE);
		if(count == 0) {
			sleep(1);
			continue;
		}

		for (j = 0 ; j < count ; j++) {
			response[j] = buf[j] ;
		}

		if ( (p_strstr = strstr(response, "ERROR")) ) {
			success = 0;              
		}

		if ( (p_strstr = strstr(response, "OK")) ) {
			success = 1;
		}

		if (success != -1)
			break;
	}
	return success;
}

static void send_atcmd(char *cmd)
{
	write(gsm_channel, cmd, strlen(cmd));
}

static void close_uart(void)
{
	close(gsm_channel);
}

void gsm_test(void)
{
	char *ret_buf = malloc(BUFFSIZE);

	poweron_process();
	modem_comm_init();
	send_atcmd("at+cfun=0\r\n");

	if (gsm_listen(ret_buf)) {
		/* XXX: do some checking */
	}

	close_uart();

	free(ret_buf);
}

