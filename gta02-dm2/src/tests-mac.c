#include "dm2.h"

static void do_wifi_on_test(void)
{
	static char buffer[BUFSIZ + 1];
	test_t *tests = suites[active_suite].tests;

	system("modprobe ar6000");

	system("ifconfig eth0 up");

	system("ifconfig | grep eth0 | grep HWaddr > /tmp/wifi_mac");

	if (access(WIFI_MAC_PATH,R_OK) != 0) {
		oltk_view_set_text(view, "Fail");
		goto err;
	}

	memset(buffer,0,sizeof(buffer));
	read_log(WIFI_MAC_PATH,buffer,BUFSIZ);
	printf("wifi_mac:\n%s\n",buffer);

	if (!strlen(buffer)){
		goto err;
	}
	else{
		if ((tests[active_test].log = strstr(buffer,"HWaddr"))==NULL){
			goto err;
		}
		else{
			oltk_view_set_text(view, tests[active_test].log);
			oltk_redraw(oltk);
			return;
		}
	}

err:
	oltk_view_set_text(view,"Fail");
	//strcpy(tests[active_test].log , "No Mac Address");
	oltk_redraw(oltk);
}

static void gsm_on_protect_block(void)
{
	countdown(GSM_TEST_TIME,FALSE);
}

static void gsm_on_test(void)
{
	static char buffer[BUFSIZ + 1];
	test_t *tests = suites[active_suite].tests;

	system("echo at+cgmr | "
	       "libgsmd-tool -m atcmd | "
	       "grep CGMR > /tmp/gsm_mac");

	if (access(GSM_MAC_PATH,R_OK)) {
		oltk_view_set_text(view, "Fail\n");
		oltk_redraw(oltk);
		return;
	}

	memset(buffer, 0, sizeof(buffer));

	read_log(GSM_MAC_PATH,buffer,BUFSIZ);
	printf("gsm:\n%s\n",buffer);

	if (!strlen(buffer))
		goto err;

	tests[active_test].log = strstr(buffer,"Moko");
	if (!tests[active_test].log)
		goto err;

	oltk_view_set_text(view, tests[active_test].log);
	oltk_redraw(oltk);

	return;

err:
	oltk_view_set_text(view,"Fail");
	//strcpy(tests[active_test].log , "No GSM Version");
	oltk_redraw(oltk);
}



static void do_gsm_on_test(void)
{
	// set_data(GSM_POWER,"0");
	// sleep(1);

	//if (!set_data(GSM_POWER,"1")){
	//	oltk_view_set_text(view,"Fail");
	//	oltk_redraw(oltk);
	//	return;
	//}

	//sleep(2);

	//system("gsmd -p /dev/ttySAC0 -s 115200 -F &");
	oltk_view_set_text(view, "Modem On");
	oltk_redraw(oltk);
	sleep(1);

	do_fork(gsm_on_protect_block);

	gsm_on_test();

	//system("kill -9 `ps | grep gsmd | awk '{print $1}'`");

}



test_t mac_address_tests[] = {
	{
		"Wifi",
		do_wifi_on_test,
		FALSE,
		NULL,
		NULL
	},
	//{ "BT", TEST_FUNCTION, do_bt_on_test } ,FALSE, NULL, NULL},
	{
		"GSM",
		do_gsm_on_test,
		FALSE,
		NULL,
		NULL
	},
	{ NULL }
};

