#include "dm2.h"

/* how many Z samples to look at, and how many have to be in limit to pass */
#define SAMPLES_CHECKED 80
#define IN_LIMIT_REQUIRED 78

/* sensor granularity is 18mG, so this is +/- 18 steps */
#define LOWER_LIMIT (1000 - (18 * 20))
#define UPPER_LIMIT (1000 + (18 * 20))

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


static int validate_log(const char * log_filepath)
{
	int i, count = 0;
	char buf[128];
	FILE *fp;
	short s;
	int n;

	fp = fopen(log_filepath, "r");
	if (!fp) {
                oltk_view_set_text(view, "Fail1a\n");
		count  = -1;
                goto bail;
	}

	printf("checking %s\n", log_filepath);

	for (i = 0; i < SAMPLES_CHECKED; i ++) {

		if (fgets(buf, sizeof(buf), fp) == NULL) {
			oltk_view_set_text(view, "Fail1b\n");
			count  = -1;
			goto bail;
		}

		if (sscanf(buf, "%x", &n) == 1) {

			/* s16 -> s32 */
			s = n;
			n = s;

			if ((n < LOWER_LIMIT) || (n > UPPER_LIMIT))
				printf("error : buf = %s / %d\n", buf, n);
			else
				count++;
		}
	}

bail:
	oltk_redraw(oltk);
	fclose(fp);

	return count;
}

static void sensor_log(void)
{
        if (access("/tmp/lis302_log1", R_OK) ||
	    access("/tmp/lis302_log2", R_OK)) {
                oltk_view_set_text(view, "Fail1\n");
                oltk_redraw(oltk);
                return;
        }

	if (validate_log("/tmp/lis302_log1") < IN_LIMIT_REQUIRED)
		goto err1;

	if (validate_log("/tmp/lis302_log2") < IN_LIMIT_REQUIRED)
		goto err2;

	oltk_view_set_text(view, "Pass");
	oltk_redraw(oltk);
	printf("Bottom sensor OK\n");
	return;

err1:
	oltk_view_set_text(view,"Top Sensor Fail");
	oltk_redraw(oltk);
	return;

err2:
	oltk_view_set_text(view,"Bottom Sensor Fail");
	oltk_redraw(oltk);
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

#ifdef CURRENTLY_UNUSED
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
#endif


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

