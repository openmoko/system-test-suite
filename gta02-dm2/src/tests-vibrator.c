#include "dm2.h"


static void do_vibrator_test(void)
{
	system("echo \"255\" > /sys/class/leds/neo1973\\:vibrator/brightness");
	oltk_view_set_text(view, "ON");
	oltk_redraw(oltk);
	sleep(VIBRATOR_TEST_TIME);
	system("echo \"0\" > /sys/class/leds/neo1973\\:vibrator/brightness");
	oltk_view_set_text(view, "OFF");
	oltk_redraw(oltk);
}



test_t vibrator_tests[] = {
	{
		"ON",
		do_vibrator_test,
		FALSE,
		NULL,
		NULL
	},
	{ NULL }
};

