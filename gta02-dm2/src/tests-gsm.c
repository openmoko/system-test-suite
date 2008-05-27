#include "dm2.h"

/* from dm2.c */
extern void audio_path(int path);

static void gsm_dial_test(void)
{
	system("libgsmd-tool -m shell < /home/root/dialout |"
		" grep -v ^# | grep -v ^libg | grep -v ^This\\  |"
		" sed \"s/)/)\\\\n/g\" >/tmp/gsm-progress");
}

static void do_gsm_dial_test(void)
{
	char buf[1024];

	unlink("/tmp/gsm-progress");

	oltk_view_set_text(view, "Modem On");
	oltk_redraw(oltk);
	sleep(1);

	audio_path(GSM_RECEIVER);

	do_fork(gsm_dial_test);

	countdown_statusfile(GSM_DIAL_TIME, TRUE, "/tmp/gsm-progress");

	system("killall libgsmd-tool");

	read_log("/tmp/gsm-progress", buf, sizeof(buf));

	if (!strlen(buf))
		strcpy(buf, "No data");

	if (strstr(buf, "CONNECTED") || strstr(buf, "SYNC") ||
		strstr(buf, "PROGRESS") || strstr(buf, "ALERT"))
		strncat(buf, "\nPASS", sizeof(buf) - 1);
	else
		strncat(buf, "\nFAIL", sizeof(buf) - 1);

	oltk_view_set_text(view, buf);
	oltk_redraw(oltk);

	sleep(1);
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

