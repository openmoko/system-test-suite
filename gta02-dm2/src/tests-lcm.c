#include "dm2.h"

static void do_rgb_test(void)
{
	static struct oltk_button *fullscreen;
	char buf[BUFSIZ];
	int i;

	for (i = 3; i > -1; i--) {
		fullscreen = oltk_button_add(oltk, 0, 0, xres, yres);
		oltk_button_set_color(fullscreen, OLTK_BUTTON_STATE_NORMAL,
						  OLTK_BUTTON_COLOR_BG,
						  (0x000000 | (0xFF << 8 * i)));
		oltk_button_set_color(fullscreen, OLTK_BUTTON_STATE_NORMAL,
						  OLTK_BUTTON_COLOR_BD,
						  (0x000000 | (0xFF << 8 * i)));

		oltk_button_show(fullscreen, 1);
		oltk_redraw(oltk);
		oltk_msleep(oltk, LCM_TEST_TIME);
		oltk_button_show(fullscreen, 0);
	}

	fullscreen = oltk_button_add(oltk, 0, 0, xres, yres);
	oltk_button_set_color(fullscreen, OLTK_BUTTON_STATE_NORMAL,
					  OLTK_BUTTON_COLOR_BG, 0xffffff);
	oltk_button_set_color(fullscreen, OLTK_BUTTON_STATE_NORMAL,
					  OLTK_BUTTON_COLOR_BD, 0xffffff);

	oltk_button_show(fullscreen, 1);
	oltk_redraw(oltk);
	oltk_msleep(oltk, LCM_TEST_TIME + 2000);
	oltk_button_show(fullscreen, 0);

	//	snprintf(buf, BUFSIZ, "%s called\n", __FUNCTION__);
	snprintf(buf, BUFSIZ, "Display Colorful Background done\n");
	oltk_view_set_text(view, buf);
	oltk_redraw(oltk);
}

/* in dm2.c */
extern int brightness_test(int level);

static void do_brightness_test(void)
{

	if (!brightness_test(1))
		return;

	oltk_view_set_text(view, "Level: 1");
	oltk_redraw(oltk);
	countdown(BRIGHTNESS_TEST_TIME,FALSE);

	if (!brightness_test(2))
		return;

	oltk_view_set_text(view, "Level: 2");
	oltk_redraw(oltk);
	countdown(BRIGHTNESS_TEST_TIME,FALSE);

	if (!brightness_test(3))
		return;

	oltk_view_set_text(view, "Level: 3");
	oltk_redraw(oltk);
	countdown(BRIGHTNESS_TEST_TIME,FALSE);

	if (!brightness_test(MAX))
		return;

	oltk_view_set_text(view, "Level: MAX");
	oltk_redraw(oltk);
}


test_t lcm_tests[] = {
	{
		"RGB",
		do_rgb_test,
		FALSE,
		NULL,
		NULL
	},
	{
		"PWM",
		do_brightness_test,
		FALSE,
		NULL,
		NULL
	},
	{ NULL }
};

