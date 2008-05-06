#include "dm2.h"
#include "nmea.h"

/* from dm2.c */
extern int set_uart_bautrate(char *device, char *speed);
extern void gps_reset(char c);
extern int set_data(const char* device ,const char* data);

/*gps.c */
const char*
nmea_epoch_end(char * buf512, struct nmea_gga* gga, struct nmea_lor* lor);


static void do_gps_test(void)
{
	char buffer[BUFSIZ + 1];
	pid_t pid;
	pid_t ppid;
	test_t *tests = suites[active_suite].tests;
	char agps_nema_data[512];
	FILE *fp = fopen(GPS_DEVICE, "r");

	resu = 0;
	fixed = 0;

	if (!fp) {
		sprintf(buffer, "unable to open %s", GPS_DEVICE);
		goto err;
	}

	/* bring GPS power down 1s and then leave up for test */

	if (!set_data(GPS_POWER, "0")) {
		strcpy(buffer,"Fail");
		goto err;
	}

	countdown(1, FALSE);

	if (!set_data(GPS_POWER, "1")) {
		strcpy(buffer, "Fail");
		goto err;
	}

	if (!set_uart_bautrate(GPS_DEVICE, "9600")) {
		strcpy(buffer,"Fail");
		//     goto err;
	}

	gps_reset('c');
	close(fd);
//	countdown(1, TRUE);

	oltk_view_set_text(view, "GPS Go");
	oltk_redraw(oltk);

	runtime_init();

	if (!(pid = fork())) {

		sleep(GPS_TEST_TIME);
		printf("\ntime out\n");

		if ( !fixed ) {
			fclose(fp);
		//	strcpy(buffer, "fail");
			printf("\ngps fail\n");
			resu = 1;
			ppid = getppid();
			kill(ppid, SIGKILL);
			return;
		}
	}

	while (!fixed) {
		struct nmea_gga gga;
		struct nmea_lor n_lor;

		if (!fgets(buffer, sizeof(buffer), fp)) {
			strcpy(buffer, "read error on gps");
			goto err;
		}

		puts(buffer);

		if (strncmp(buffer, "$GPGGA", 6))
			continue;

		GPGGA(buffer, &gga);
		fixed = atoi(gga.fix_quality);

		nmea_epoch_end(agps_nema_data, &gga, &n_lor);

		if (!fixed) {
			oltk_view_set_text(view, agps_nema_data);
			oltk_redraw(oltk);
		}
	}

	printf("info :%s\n", agps_nema_data);
	oltk_view_set_text(view, agps_nema_data);
	oltk_redraw(oltk);
	kill(pid, SIGKILL);
	goto bail;

err:
	oltk_view_set_text(view, buffer);
	oltk_redraw(oltk);
	tests[active_test].log = buffer;

bail:
	fclose(fp);
}


test_t gps_tests[] = {
	{
		"Internal",
		do_gps_test,
		FALSE,
		NULL,
		NULL
	},
	{
		"External",
		do_gps_test,
		FALSE,
		NULL,
		NULL
	},
	{ NULL }
};

