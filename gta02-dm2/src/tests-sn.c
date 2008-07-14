#include "dm2.h"
#include "nmea.h"

extern int table_prn_sn[];
int already_beep;

/* from dm2.c */
int set_uart_bautrate(char *device, char *speed);
void gps_reset(int fd, char c);
int set_data(const char* device ,const char* data);

static void do_gps_sn_test(void);

char cnr_buff[128];

test_t sn_tests[] = {
	{
		"CN",
		do_gps_sn_test,
		FALSE,
		NULL,
		NULL
	},
	{ NULL }
};

static int get_gps_sn_data(FILE *fp, char * buf, int size)
{
	int fixed1 = 0;
	char temp[256];

	*buf = '\0';

	if (!fp) {
		snprintf(buf, size, "[ERROR]Fail to open device interface!\n");
		printf("%s\n", buf);
		return !fixed1;
	}

	if (!fgets(temp, sizeof(temp), fp)) {
		sprintf(buf, "[ERROR]Fail to read NMEA 0813 Messages!\n");
		return fixed1;
	}

	if (strncmp(temp, GSV_SENTENCE_ID, 6))
		return 0;

	nmea_process2(buf, temp, &fixed1, cnr_buff);
	printf("Translator: %s\n",buf);

	return fixed1;
}


static void do_gps_sn_test(void)
{
	char buffer[BUFSIZ + 1];
	int i;
        pid_t pid;
	pid_t ppid;
	fixed = 0;
	test_t *tests = suites[active_suite].tests;
	int fd;

	if (!set_data(GPS_POWER, "0")) {
		strcpy(buffer,"Fail");
		goto err;
	}

	sleep(1);

	fd = set_uart_bautrate(GPS_DEVICE, "9600");
	if (!fd){
		strcpy(buffer,"Fail");
		goto err;
	}


	/* huh we power it up then put it in shielding box? */

	if (!set_data(GPS_POWER, "1")) {
		strcpy(buffer,"Fail");
		goto err;
	}

	oltk_view_set_text(view, "Put into shielding box now.");
	oltk_redraw(oltk);
	sleep(2);

	countdown(5, TRUE);

	gps_reset(fd, 'c');

	close(fd);

	countdown(1, FALSE);

	oltk_view_set_text(view, "GPS Go");
	oltk_redraw(oltk);

	already_beep = 0;
	memset(buffer, 0, sizeof(buffer));

	runtime_init();

	for (i = 0; i < 32; i++)
		table_prn_sn[i] = -1;

	if ((pid = fork())) {
		FILE *fp = fopen(GPS_DEVICE, "r");

                while (1) {
                        fixed = get_gps_sn_data(fp, buffer, BUFSIZ);
                        if (strlen(buffer)) {
				printf("fixed:%d\n",fixed);
                                oltk_view_set_text(view, buffer);
                                oltk_redraw(oltk);

				if (fixed) {
					fclose(fp);
					kill(pid, SIGKILL);
					sn_tests[0].log=cnr_buff;
					return;
				}
                        }
                }
        } else {
		sleep(GPS_SNTEST_TIME);
                printf("\ntime out\n");
                if (!fixed) {
                        //strcpy(buffer, "fail");
                        ppid = getppid();
			kill(ppid, SIGKILL);
                        printf("\ngps fail\n");
			system("alsactl -f /etc/play_wav_speaker.state restore");
			system("aplay /usr/share/dm2/bru93q_7s.wav");
			oltk_view_set_text(view, "fail");
                        oltk_redraw(oltk);
                        return;
                }
	}
err:
	tests[active_test].log=buffer;

	oltk_view_set_text(view, buffer);
	oltk_redraw(oltk);

}


