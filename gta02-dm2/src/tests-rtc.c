#include "dm2.h"

static void do_get_time(void)
{
	struct tm *p;
	time_t timep;
	char buf[BUFSIZ];
	int i;

	for (i = 0; i < RTC_TEST_TIME; i++) {
		time(&timep);
		p = localtime(&timep);
		snprintf(buf, BUFSIZ, "%d/%d/%d %d:%d:%d\n",
				      1900 + p->tm_year,
				      1 + p->tm_mon,
				      p->tm_mday,
				      p->tm_hour,
				      p->tm_min,
				      p->tm_sec);

		oltk_view_set_text(view, buf);
		oltk_redraw(oltk);
		oltk_msleep(oltk, 1000);
	}
}

test_t rtc_tests[] = {
	{
		"Read RTC",
		do_get_time,
		FALSE,
		NULL,
		NULL
	},
	{ NULL }
};
