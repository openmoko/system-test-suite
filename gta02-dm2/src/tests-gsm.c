#include "dm2.h"

/* from dm2.c */
extern void audio_path(int path);

static void gsm_dial_test(void)
{
	static char buffer[BUFSIZ + 1];

	system("libgsmd-tool -m shell < /home/root/dialout");
}

static void do_gsm_dial_test(void)
{
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

	audio_path(GSM_RECEIVER);

	do_fork(gsm_dial_test);
	countdown(GSM_DIAL_TIME,TRUE);

	sleep(1);

	//system("kill -9 `ps | grep gsmd | awk '{print $1}'`");

	audio_path(SPEAKER);
}


test_t gsm_tests[] = {
	/* {
		"ON",

		do_gsm_on_test,
		FALSE,
		NULL,
		NULL
		}, */
	{
		"Dial Out",
		do_gsm_dial_test,
		FALSE,
		NULL,
		NULL
	},
	{ NULL }
};

