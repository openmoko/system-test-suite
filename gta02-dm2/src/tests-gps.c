#include <stdlib.h>
#include "dm2.h"
#include "nmea.h"

/* from dm2.c */
extern int set_uart_bautrate(char *device, char *speed);
extern void gps_reset(int fd, char c);
extern int set_data(const char* device ,const char* data);
extern int table_prn_sn[32];

/*gps.c */
const char*
nmea_epoch_end(char * buf512, struct nmea_gga *gga, struct nmea_lor *lor,
		struct nmea_zda *zda, char *fixed_time);

char fixed_time[32];
static void do_gps_test(int antenna);

static inline int cmpint (const void *p1, const void *p2)
{
	return (*(int *)p1 > *(int *)p2? 1:0);
}

static void do_gps_test_external(void){
	do_gps_test(0);
}

static void do_gps_test_internal(void){
	do_gps_test(1);
}

test_t gps_tests[] = {
	{
		"External",
		do_gps_test_external,
		FALSE,
		NULL,
		NULL
	},
	{
		"Internal",
		do_gps_test_internal,
		FALSE,
		NULL,
		NULL
	},
	{ NULL }
};

static void do_gps_test(int antenna)
{
	char buffer[BUFSIZ + 1];
	pid_t pid;
	pid_t ppid;
	test_t *tests = suites[active_suite].tests;
	char agps_nema_data[512], tmp[16];
	FILE *fp;
	struct nmea_gga gga;
	struct nmea_lor n_lor;
	struct nmea_zda zda;
	struct nmea_gsv gsv;
	int fd, i, avg_cn = 0;

	resu = 0;
	fixed = 0;
	memset(&zda, 0, sizeof(struct nmea_zda));

	/* bring GPS power down 1s and then leave up for test */

	if (!set_data(GPS_POWER, "0")) {
		strcpy(buffer,"Fail");
		goto err;
	}

	countdown(3, FALSE);

	if (!set_data(GPS_POWER, "1")) {
		strcpy(buffer, "Fail");
		goto err;
	}

	fd = set_uart_bautrate(GPS_DEVICE, "9600");
	if (!fd) {
		strcpy(buffer,"Fail");
		//     goto err;
	}

	gps_reset(fd, 'c');
	close(fd);

	fp = fopen(GPS_DEVICE, "r");
	if (!fp) {
		sprintf(buffer, "unable to open %s", GPS_DEVICE);
		goto err;
	}
//	countdown(1, TRUE);

	oltk_view_set_text(view, "GPS Go");
	oltk_redraw(oltk);

	runtime_init();

	for (i = 0; i < 32; i++)
		table_prn_sn[i] = -1;

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
		if (!fgets(buffer, sizeof(buffer), fp)) {
			strcpy(buffer, "read error on gps");
			goto err;
		}

		puts(buffer);

		if (!strncmp(buffer, GGA_SENTENCE_ID, 6)) {
			GPGGA(buffer, &gga);
			fixed = atoi(gga.fix_quality);
			nmea_epoch_end(agps_nema_data, &gga, &n_lor, &zda, fixed_time);
			/* we stop updating the display if we feel we had a fix */
			if (!fixed) {
				oltk_view_set_text(view, agps_nema_data);
				oltk_redraw(oltk);
			}
		}

		if (!strncmp(buffer, ZDA_SENTENCE_ID, 6))
			nmea_to_ZDA(buffer, &zda); /* update next second, never mind */

		if (!strncmp(buffer, GSV_SENTENCE_ID, 6))
			GPGSV(buffer, &gsv);

	}

	qsort(table_prn_sn, 32, sizeof(int), cmpint);

	/* obtain the average CN of top 4 CN */
	for (i=0; i<3; i++)
		avg_cn += table_prn_sn[29+i];

	avg_cn = (avg_cn/3);

	sprintf(tmp, "\nAvg CN: %d", avg_cn);
	strcat(agps_nema_data, tmp);
	strcat(fixed_time, tmp);

	gps_tests[antenna].log=fixed_time;
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

