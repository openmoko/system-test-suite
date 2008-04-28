#include "dm2.h"


static void bt_mac_address(vodi)
{
	static char buffer[BUFSIZ + 1];
        test_t *tests = suites[active_suite].tests;

	system("hcitool dev > /tmp/bt_mac");

	if (access(BT_MAC_PATH,R_OK) != 0) {
		oltk_view_set_text(view, "Fail\n");
		oltk_redraw(oltk);
		return;
	}

	memset(buffer,0,sizeof(buffer));

	read_log(BT_MAC_PATH,buffer,BUFSIZ);
	printf("Bt:\n%s\n",buffer);

	if (!strlen(buffer))
		goto err;

	if ((tests[active_test].log = strstr(buffer,"hci0"))==NULL)
		goto err;

	oltk_view_set_text(view, tests[active_test].log);
	oltk_redraw(oltk);
	return;

err:
	oltk_view_set_text(view,"Fail");
	//strcpy(tests[active_test].log , "No Mac Address");
	oltk_redraw(oltk);
}

static void do_bt_on_test(void)
{
	if (!set_data(BT_POWER,"1")){
		oltk_view_set_text(view,"Fail");
		oltk_redraw(oltk);
		return;
	}

	sleep(1);

	if (!set_data(BT_RESET,"0")){
		oltk_view_set_text(view,"Fail");
		oltk_redraw(oltk);
		return;
	}

	sleep(2);

	system("hciconfig hci0 up");
	sleep(2);

	bt_mac_address();
}

static void bt_scan_test(void)
{
	system("hcitool scan > /tmp/bt_log");
}

static void bt_scan_log(void)
{
	static char buffer[BUFSIZ + 1];
	test_t *tests = suites[active_suite].tests;
	int i;

	if (access(BT_LOG_PATH,R_OK)) {
		oltk_view_set_text(view, "Fail\n");
		oltk_redraw(oltk);
		return;
	}

	memset(buffer,0,sizeof(buffer));

	read_log(BT_LOG_PATH,buffer,BUFSIZ);
	printf("Bt_log:\n%s\n",buffer);
/*
	for(i=0;buffer[i]=0;i++) {
		if ((buffer[i] >= 0x61 && buffer[i] <= 0x7A) ||
				(buffer[i] >= 0x41 && buffer[i]>= 0x5A) ||
				(buffer[i] >= 0x30 && buffer[i]<= 0x3A) ||
				(buffer[i] == "00")) {
			;
		}
		else
			buffer[i] = '?';
	}
	printf("Bt_log:\n%s\n",buffer);
*/
	if ((tests[active_test].log = strstr(buffer, "00"))==NULL){
		oltk_view_set_text(view,"Fail");
		//strcpy(tests[active_test].log , "No Device");
	}
	else
		oltk_view_set_text(view, tests[active_test].log);

	oltk_redraw(oltk);
}

static void do_bt_scan_test(void)
{

	do_fork(bt_scan_test);

	countdown(BT_TEST_TIME,TRUE);

	bt_scan_log();

}


test_t bt_tests[] = {
	{
		"On",
		do_bt_on_test,
		FALSE,
		NULL,
		NULL
	},
	{
		"Scan",
		do_bt_scan_test,
		FALSE,
		NULL,
		NULL
	},
	/* {
		"Connect",
		do_bt_connect_test,
		FALSE,
		NULL,
		NULL
	}, */
	{ NULL }
};

