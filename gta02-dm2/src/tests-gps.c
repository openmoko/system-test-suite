#include "dm2.h"

/* from dm2.c */
extern int set_uart_bautrate(char *device, char *speed);
extern void gps_reset(char c);
extern int set_data(const char* device ,const char* data);
extern int get_gps_data(char* device,char* buf);

static void do_gps_test(void)
{
	char buffer[BUFSIZ + 1];
	int i;//,fixed;
	resu = 0;
	fixed = 0;
	pid_t pid;
	pid_t ppid;
	test_t *tests = suites[active_suite].tests;

	if (!set_data(GPS_POWER,"0")){
		strcpy(buffer,"Fail");
		goto err;
	}

	countdown(5, FALSE);

	if (!set_data(GPS_POWER,"1")) {
		strcpy(buffer,"Fail");
		goto err;
	}

	if (!set_uart_bautrate(GPS_DEVICE,"9600")) {
		strcpy(buffer,"Fail");
		//     goto err;
	}

	gps_reset('c');

	close(fd);
#if 1
	countdown(1, TRUE);

	oltk_view_set_text(view, "GPS Go");
	oltk_redraw(oltk);

	//printf("GPS Go\n");

	runtime_init();

	memset(buffer, 0, sizeof(buffer));

	if ( !(pid = fork()) ) {

		sleep(GPS_TEST_TIME);
		printf("\ntime out\n");

		if ( !fixed ) {
		//	strcpy(buffer, "fail");
			printf("\ngps fail\n");
			resu = 1;
			ppid = getppid();
			kill(ppid, SIGKILL);
			return;
		}
	}

	while (1) {
		fixed = get_gps_data(GPS_DEVICE, buffer);
		printf("fixed:%d\n",fixed);

		if (!strlen(buffer))
			continue;

		if (!fixed) {
			oltk_view_set_text(view, buffer);
			oltk_redraw(oltk);
		}

		/* FIXME: huh what is 520? */
		if (fixed && fixed != 520) {
			//break;
			printf("info :%s\n", buffer);

			oltk_view_set_text(view, buffer);
			oltk_redraw(oltk);
			kill(pid, SIGKILL);
			return;
		}
	}

	return ;
	/*
	   if (!set_data(GPS_POWER,"0")){
	   strcpy(buffer,"Fail");
	   }*/

err:
	oltk_view_set_text(view, buffer);
	oltk_redraw(oltk);
	tests[active_test].log=buffer;

#endif
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

