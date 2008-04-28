#include "dm2.h"

static int get_voltage(const char* device ,int type)
{
	FILE *fp = NULL;
	char battery_voltage_value[8];
	int i;
	float total = 0, average = 0;
	char buffer[8];

	/* Confirm device initialized */
	if (access(device, R_OK))
		goto err;

	for (i = 0; i< 10; i++)
		if ((fp = fopen(device, "r"))) {
			if (fread(battery_voltage_value, sizeof(char), 5, fp))
				total += atoi(battery_voltage_value);
			else {
				fclose(fp);
				break;
			}
		} else
			goto err;

	fclose(fp);

	if (i == 10) {
		average = total / 10;
		if (total) {
			if (type != BATTERY)
				average = ((average* 5 /875) - 0.22);

			printf("Battery:%2f mV\n",average);
			sprintf(buffer,"%2f mV\n",average);
			oltk_view_set_text(view, buffer);
		} else
			oltk_view_set_text(view, "0 V");

		return 1;

	}
err:
	oltk_view_set_text(view, "Fail\n");
	return 0;
}



static void do_battery_test(void)
{
	get_voltage(BATTERY_VOLTAGE,BATTERY);
	oltk_redraw(oltk);
}

static void do_ac_test(void)
{
	get_voltage(BATTERY_VOLTAGE,DC);
	oltk_redraw(oltk);
}

extern int brightness_test(int level);

static void do_suspend_test(void)
{
	oltk_view_set_text(view, "Please Wait 10 seconds to wake up");
	oltk_redraw(oltk);
	brightness_test(1);

	// system("apm -s");
	// system("echo mem > /sys/power/state");
	system("cp /home/root/suspend-workaround.sh /tmp/");
	system("./suspend-workaround.sh");

	// oltk_view_set_text(view, "Wake Up Success");
	oltk_view_set_text(view, "Not yet");
	oltk_redraw(oltk);
	brightness_test(MAX);
}

static void do_hdq_test(void)
{
	static char buffer[BUFSIZ + 1];
	test_t *tests = suites[active_suite].tests;

	system("/home/root/test_script hdq detect > /tmp/hdq_log");

	if (access("/tmp/hdq_log", R_OK)) {
		oltk_view_set_text(view, "Fail\n");
		oltk_redraw(oltk);
		return;
	}

	memset(buffer, 0, sizeof(buffer));

	read_log("/tmp/hdq_log", buffer, BUFSIZ);
	printf("HDQ:\n%s\n",buffer);

	if (!strlen(buffer))
		goto err;

	tests[active_test].log=buffer;
	oltk_view_set_text(view, tests[active_test].log);
	oltk_redraw(oltk);
	return;

err:
	oltk_view_set_text(view,"Fail");
	oltk_redraw(oltk);
}


test_t pmu_tests[] = {
	{
		"Battery",
		do_battery_test,
		FALSE,
		NULL,
		NULL
	},
	/*{
		"DC",
		do_ac_test },
		FALSE,
		NULL,
		NULL
	}, */
	{
		"HDQ",
		do_hdq_test,
		FALSE,
		NULL,
		NULL
	},
	/* {
		"Suspend",
		do_suspend_test,
		FALSE,
		NULL,
		NULL
	}, */
	{ NULL }
};

