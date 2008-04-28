#include "dm2.h"

static void sensor1_test(void)
{
	system("hexdump /dev/input/event2 | "
	       "grep \".* .* .* .* .* 0002 0002 .* .*$\" | "
	       "cut -d' ' -f8 > /tmp/lis302_log1");
}

static void sensor2_test(void)
{
	system("hexdump /dev/input/event3 | "
	       "grep \".* .* .* .* .* 0002 0002 .* .*$\" | "
	       "cut -d' ' -f8 > /tmp/lis302_log2");
}

static int check_sensor(char *buffer)
{
	/* FIXME: convert hex -> dec then range check */
	if (strstr(buffer,"038")==NULL) {
		if (strstr(buffer,"039")==NULL){
			if (strstr(buffer,"03a")==NULL){
				if (strstr(buffer,"03b")==NULL){
					if (strstr(buffer,"03c")==NULL){
						if (strstr(buffer,"03d")==NULL){
							if (strstr(buffer,"03e")==NULL){
								if (strstr(buffer,"037")==NULL){
									if (strstr(buffer,"03f0")==NULL){
										if (strstr(buffer,"03f1")==NULL){
											if (strstr(buffer,"03f2")==NULL)
												return 0;
											else
												return 1;
										}
										else
											return 1;
									}
									else
										return 1;
								}
								else
									return 1;
							}
							else
								return 1;
						}
						else
							return 1;
					}
					else
						return 1;
				}
				else
					return 1;
			}
			else
				return 1;
		}
		else
			return 1;
	}
	else
		return 1;
}




static void sensor_log(void)
{
        static char buffer[BUFSIZ + 1];
	test_t *tests = suites[active_suite].tests;
	int i, j, count = 0, suc_cnt = 80;
	static char buf[5];

        if (access("/tmp/lis302_log1", R_OK) ||
	    access("/tmp/lis302_log2", R_OK)) {
                oltk_view_set_text(view, "Fail1\n");
                oltk_redraw(oltk);
                return;
        }

        memset(buffer, 0, sizeof(buffer));

        read_log("/tmp/lis302_log1",buffer,BUFSIZ);
//        printf("motion sensor1:\n%s\n",buffer);
	printf("sensor1\n");

        if (!strlen(buffer))
                goto err;

	count = 0;
	for (i = 0; i < strlen(buffer); i += 5) {
		memset(buf, 0, sizeof(buf));
		for (j = 0; j < 5; j++)
			buf[j] = buffer[i+j];
		buf[4] = 0;

		if (check_sensor(buf))
			count++;
		else
			printf("error : buf = %s\n", buf);
	}

	printf("count = %d\n", count);

	if (count > suc_cnt)
		goto sensor2;
	else
		goto err1;

sensor2:
	printf("Top sensor OK\n");

        memset(buffer, 0, sizeof(buffer));

        read_log("/tmp/lis302_log2",buffer,BUFSIZ);
//	printf("motion sensor2:\n%s\n",buffer);
	printf("sensor2\n");

        if (!strlen(buffer))
                goto err;

	count = 0;
	for (i = 0; i < strlen(buffer); i += 5) {
		memset(buf, 0, sizeof(buf));
		for (j = 0; j < 5; j++)
			buf[j] = buffer[i+j];
		buf[4] = 0;
	//	printf("buf = %s\n", buf);

		if (check_sensor(buf))
			count++;
		else
			printf("error : buf = %s\n", buf);
	}
	printf("count = %d\n", count);

	if (count > suc_cnt)
		goto pass;
	else
		goto err2;

err:
        oltk_view_set_text(view,"Fail");
        oltk_redraw(oltk);
	return;

err1:
	oltk_view_set_text(view,"Top Sensor Fail");
	oltk_redraw(oltk);
	return;

err2:
	oltk_view_set_text(view,"Bottom Sensor Fail");
	oltk_redraw(oltk);
	return;

pass:
	oltk_view_set_text(view, "Pass");
	oltk_redraw(oltk);
	printf("Bottom sensor OK\n");
	return;
}



static void do_sensor_test(void)
{
	system("rm /tmp/lis302_log*");

        do_fork(sensor1_test);
	do_fork(sensor2_test);
        countdown(1, FALSE);

	system("kill -9 `ps | grep hexdump | awk '{print $1}'`");

	sleep(1);

        sensor_log();
}

static void do_motion_sensor_test(void)
{
	static char buffer[BUFSIZ + 1];
	static char tmp[BUFSIZ*2 + 1];
	test_t *tests = suites[active_suite].tests;

	system("cat /proc/bus/input/devices | grep lis302 > /tmp/ms_log");

	if (access("/tmp/ms_log", R_OK) != 0) {
		oltk_view_set_text(view, "Fail1\n");
		oltk_redraw(oltk);
		return;
	}

	memset(buffer, 0, sizeof(buffer));

	read_log("/tmp/ms_log", buffer, BUFSIZ);
	printf("motion sensor:\n%s\n", buffer);
	sprintf(tmp, "\n%s\n", buffer);

	if (!strlen(buffer))
		goto err;

	tests[active_test].log = strstr(buffer, "lis302");
	if (tests[active_test].log == NULL)
		goto err;

	tests[active_test].log = tmp;
	oltk_view_set_text(view, tests[active_test].log);
	oltk_redraw(oltk);
	return;

err:
	oltk_view_set_text(view,"Fail");
	oltk_redraw(oltk);
}



test_t ms_tests[] = {
	/* {
		"Detect",
		do_motion_sensor_test,
		FALSE,
		NULL,
		NULL
	}, */
	{
		"Test",
		do_sensor_test,
		FALSE,
		NULL,
		NULL
	},
	{ NULL }
};

