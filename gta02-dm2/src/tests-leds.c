#include "dm2.h"



static void do_orange_led_test(void)
{
	int count = LED_TEST_TIME;

	/* Confirm device initialized */
	if (access(ORANGE_LED_DEVICE, R_OK) != 0) {
		oltk_view_set_text(view, "Fail\n");
		oltk_redraw(oltk);
		return;
	}

	while(count)
	{
		system("echo \"255\" > /sys/class/leds/gta02-power\\:orange/brightness");
		oltk_view_set_text(view, "ON");
		oltk_redraw(oltk);
		sleep(1);

		system("echo \"0\" > /sys/class/leds/gta02-power\\:orange/brightness");
		oltk_view_set_text(view, "OFF");
		oltk_redraw(oltk);
		sleep(1);

		count--;
	}
}

static void do_blue_led_test(void)
{
	int count = LED_TEST_TIME;

	/* Confirm device initialized */
	if (access(BLUE_LED_DEVICE, R_OK) != 0) {
		oltk_view_set_text(view, "Fail\n");
		oltk_redraw(oltk);
		return;
	}

	while(count)
	{
		system("echo 255 > /sys/class/leds/gta02-power\\:blue/brightness");
		oltk_view_set_text(view, "ON");
		oltk_redraw(oltk);
		sleep(1);

		system("echo 0 > /sys/class/leds/gta02-power\\:blue/brightness");
		oltk_view_set_text(view, "OFF");
		oltk_redraw(oltk);
		sleep(1);

		count--;
	}
}

static void do_red_led_test(void)
{
	int count = LED_TEST_TIME;

	/* Confirm device initialized */
	if (access(RED_LED_DEVICE, R_OK) != 0) {
		oltk_view_set_text(view, "Fail\n");
		oltk_redraw(oltk);
		return;
	}

	while(count)
	{
		system("echo \"255\" > /sys/class/leds/gta02-aux\\:red/brightness");
		oltk_view_set_text(view, "ON");
		oltk_redraw(oltk);
		sleep(1);

		system("echo \"0\" > /sys/class/leds/gta02-aux\\:red/brightness");
		oltk_view_set_text(view, "OFF");
		oltk_redraw(oltk);
		sleep(1);

		count--;
	}
}


test_t led_tests[] = {
	{
		"Red Blink",
		do_red_led_test,
		FALSE,
		NULL,
		NULL
	},
	{
		"Blue Blink",
		do_blue_led_test,
		FALSE,
		NULL,
		NULL
	},
	{
		"Orange Blink",
		do_orange_led_test,
		FALSE,
		NULL,
		NULL
	},
	{ NULL }
};
