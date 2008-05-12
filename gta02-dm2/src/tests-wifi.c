#include "dm2.h"

static void do_wifi_test(void)
{
	char buf[400];
	test_t *tests = suites[active_suite].tests;

	oltk_view_set_text(view, "");
	oltk_redraw(oltk);

	unlink("/tmp/wlan-progress"); /* ignore any error from not existing */
	system("/home/root/test_script wifi setSSID &");

	countdown_statusfile(WIFI_TEST_TIME, TRUE, "/tmp/wlan-progress");

	if (access(WIFI_LOG_PATH, R_OK)) {
		oltk_view_set_text(view, "Fail\n");
		oltk_redraw(oltk);
		return;
	}

	system("killall -q udhcpc");
	system("killall -q test_script");

	memset(buf, 0, sizeof(buf));
	read_log("/tmp/wlan-progress", buf, sizeof(buf));
	printf("WiFi:\n%s\n",buf);
	if (!strlen(buf))
		strcpy(buf, "No data");
	if (!strstr(buf, "SUCCESS")) {
		strncat(buf, "\nFAIL", sizeof(buf) - 1);
		oltk_view_set_text(view, buf);
		oltk_redraw(oltk);
	}
	tests[active_test].log = buf;
}

test_t wifi_tests[] = {
	/* {
		"ON",
		do_wifi_on_test,
		FALSE,
		NULL,
		NULL
	}, */
	{
		"Transmit",
		do_wifi_test,
		FALSE,
		NULL,
		NULL
	},
	{ NULL }
};

