#include "dm2.h"

extern int table_prn_cn[];
int already_beep;

/* from dm2.c */
int set_uart_bautrate(char *device, char *speed);
void gps_reset(char c);
int set_data(const char* device ,const char* data);


static int get_gps_cn_data(char* device,char * buf, int size)
{
	FILE *fp;
	int fixed1 = 0;
	int chars_read = 0;

	char temp[64 + 1];

	fp = fopen(device, "r");

	//printf("open:%s\n",device);

	if (!fp) {
		snprintf(buf, size, "[ERROR]Fail to open device interface!\n");
		printf("%s\n", buf);
		return !fixed1;
	}
	chars_read = fread(temp, sizeof(char), 64, fp);
	if (chars_read > 0) {
		const char* agps_nema_data = agps_nmea_process2(temp, &fixed1);
		if (agps_nema_data)
			sprintf(buf,agps_nema_data);
	} else
		sprintf(buf, "[ERROR]Fail to read NMEA 0813 Messages!\n");

	printf("Translator:%s\n",buf);
	fclose(fp);

	return fixed1;
}


static void do_gps_cn_test(void)
{
	char buffer[BUFSIZ + 1];
	int i;
        pid_t pid;
	pid_t ppid;
	fixed = 0;
	test_t *tests = suites[active_suite].tests;

	if (!set_data(GPS_POWER,"0")) {
		strcpy(buffer,"Fail");
		goto err;
	}

	sleep(10);

	if (!set_data(GPS_POWER,"1")) {
		strcpy(buffer,"Fail");
		goto err;
	}

	oltk_view_set_text(view, "Put into shielding box now.");
	oltk_redraw(oltk);
	sleep(2);

	countdown(5, TRUE);

	/* Cold Start */
	if (!set_uart_bautrate(GPS_DEVICE,"9600")){
		strcpy(buffer,"Fail");
		goto err;
	}

	gps_reset('c');

	close(fd);

	countdown(1,FALSE);

	oltk_view_set_text(view, "GPS Go");
	oltk_redraw(oltk);

	already_beep = 0;
	memset(buffer, 0, sizeof(buffer));

	runtime_init();

	for (i = 0; i < 32; i++)
		table_prn_cn[i] = -1;

	if ((pid = fork())) {
                while (1) {
                        fixed = get_gps_cn_data(GPS_DEVICE, buffer, BUFSIZ);

                        printf("fixed:%d\n",fixed);

                        if (strlen(buffer)) {
                                oltk_view_set_text(view, buffer);
                                oltk_redraw(oltk);

				if (fixed) {
					kill(pid, SIGKILL);
					return;
				}
                        }
                }
        } else {
		sleep(GPS_CNTEST_TIME);
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



test_t cn_tests[] = {
	{
		"CN",
		do_gps_cn_test,
		FALSE,
		NULL,
		NULL
	},
	{ NULL }
};

