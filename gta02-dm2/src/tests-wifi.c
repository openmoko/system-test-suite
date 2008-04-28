#include "dm2.h"

static void wifi_test(void)
{
	system("/home/root/test_script wifi setSSID &");
}

static void do_wifi_test(void)
{
	char buf[BUFSIZ + 1];
	test_t *tests = suites[active_suite].tests;

	system("/home/root/test_script wifi setSSID &");

	//do_fork(wifi_test);
	countdown(WIFI_TEST_TIME, TRUE);

	if (access(WIFI_LOG_PATH,R_OK) != 0) {
		oltk_view_set_text(view, "Fail\n");
		oltk_redraw(oltk);
		return;
	}

	memset(buf,0,sizeof(buf));

	read_log(WIFI_LOG_PATH,buf,BUFSIZ);
	printf("WiFi:\n%s\n",buf);
	if (strlen(buf))
		oltk_view_set_text(view, buf);
	else {
		strcpy(buf,"No data");
		oltk_view_set_text(view, buf);
	}
	oltk_redraw(oltk);

	tests[active_test].log = buf;
	// write_log(Log_Path,buf,"a+");
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

