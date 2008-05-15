#include "dm2.h"
#include <sys/time.h>
#include <sys/select.h>
#include <math.h>

#define SAMPLES_NEEDED 100
#define BAD_SAMPLES_ALLOWED 2

/* sensor granularity is 18mG, so this is +/- 18 steps */
#define TOLERANCE (8 * 18)

void oltk_fill_rect(struct oltk *oltk, int x, int y, unsigned int width, unsigned int height, int color);

#if 0
static void do_motion_sensor_test(void)
{
	int fdtop, fdbot;
	struct input_event ev;
	fd_set fds;
	struct timeval tv;
	int coords[2][3]; /* two sets of X, Y, Z */
	int drew[2][3]; /* two sets of X, Y, Z */
	int last_update_sec = 0;
	int updates_before_exit = 1000;
	int n;
	struct oltk_button *fullscreen;
	static unsigned int colour[2] = { 0xffffffff, 0x6080c0ff };

	fdtop = open("/dev/input/event2", O_RDONLY, 0);
	if (fdtop < 0)
		goto err1;

	fdbot = open("/dev/input/event3", O_RDONLY, 0);
	if (fdbot < 0)
		goto err2;

	fullscreen = oltk_button_add(oltk, 0, 0, 480, 640);

	oltk_button_set_color(fullscreen, OLTK_BUTTON_STATE_NORMAL,
						OLTK_BUTTON_COLOR_BG,
						0x000000);
	oltk_button_set_color(fullscreen, OLTK_BUTTON_STATE_NORMAL,
						OLTK_BUTTON_COLOR_BD,
						0x000000);

	oltk_button_show(fullscreen, 1);
	oltk_redraw(oltk);

	memset(coords, 0, sizeof(coords));
	memset(drew, 0, sizeof(drew));
	/* get some motion input data stored up */
	usleep(100000);

	while (1) {
		FD_ZERO(&fds);
		FD_SET(fdtop, &fds);
		FD_SET(fdbot, &fds);
		tv.tv_sec = 0;
		tv.tv_usec = 50000;

		if (select(fdbot + 1, &fds, NULL, NULL, &tv) <= 0)
			goto again;

		if (FD_ISSET(fdtop, &fds)) {
			read (fdtop, &ev, sizeof (struct input_event));
			if (ev.type == 2)
				coords[0][ev.code] = ev.value;
		}
		if (FD_ISSET(fdbot, &fds)) {
			read (fdbot, &ev, sizeof (struct input_event));
			if (ev.type == 2)
				coords[1][ev.code] = ev.value;
		}

		gettimeofday(&tv, NULL);
		if ((tv.tv_sec * 20 + tv.tv_usec / 50000) == last_update_sec)
			continue;

		for (n = 0; n < 2; n++) {
			oltk_fill_rect(oltk, drew[n][0], drew[n][1],
					     drew[n][2], drew[n][2], 0);

			/* normalized both -1000 .... 1000 */

			drew[n][0] = ( coords[n][0]);
			drew[n][1] = (-coords[n][1]);
#if 0
			if (!n) {  /* adjust for -45 degree top sensor angle */
				drew[n][0] = (drew[n][0] *  707 / 1000) -
					     (drew[n][1] * -707 / 1000);
				drew[n][1] = (drew[n][0] * -707 / 1000) +
					     (drew[n][1] *  707 / 1000);
			}
#endif
			/* scale to screen */
			drew[n][0] = ((drew[n][0] * 240) / 1000) + 240;
			drew[n][1] = ((drew[n][1] * 320) / 1000) + 320;

			/* non-negative clip */
			if (drew[n][0] < 0)
				drew[n][0] = 0;
			if (drew[n][1] < 0)
				drew[n][1] = 0;

			drew[n][2] = (coords[n][2] + 1000) / 50;

			if (drew[n][2] < 2)
				drew[n][2] = 2;
			if (drew[n][2] > 40)
				drew[n][2] = 40;

			if ((drew[n][0] + drew[n][2]) > 479)
				drew[n][0] = 479 - drew[n][2];
			if ((drew[n][1] + drew[n][2]) > 639)
				drew[n][1] = 639 - drew[n][2];

			oltk_fill_rect(oltk, drew[n][0], drew[n][1], drew[n][2],
					drew[n][2], colour[n]);


			last_update_sec = (tv.tv_sec * 20 + tv.tv_usec / 50000);
		}

again:
		if (!updates_before_exit--) {
			close(fdtop);
			close(fdbot);

			oltk_button_show(fullscreen, 0);

			oltk_view_set_text(view, "Done");
			oltk_redraw(oltk);
			return;
		}
	}

	oltk_view_set_text(view, "Pass");
	oltk_redraw(oltk);
	return;

err1:
	oltk_view_set_text(view,"Top Sensor Fail");
	oltk_redraw(oltk);
	return;

err2:
	oltk_view_set_text(view,"Bottom Sensor Fail");
	oltk_redraw(oltk);
}
#endif


static void do_sensor_test(void)
{
	int fdtop, fdbot;
	struct input_event ev;
	fd_set fds;
	struct timeval tv;
	int coords[2][3]; /* two sets of X, Y, Z */
	int seen[2], fail[2];
	int updates_before_exit = SAMPLES_NEEDED * 10;
	char buf[200];
	int i = 0;

	fdtop = open("/dev/input/event2", O_RDONLY, 0);
	if (fdtop < 0)
		goto err1;

	fdbot = open("/dev/input/event3", O_RDONLY, 0);
	if (fdbot < 0)
		goto err2;

	memset(coords, 0, sizeof(coords));
	memset(seen, 0, sizeof(seen));
	memset(fail, 0, sizeof(fail));

	/* get some motion input data stored up */
	usleep(10000);

	while (1) {
		FD_ZERO(&fds);
		FD_SET(fdtop, &fds);
		FD_SET(fdbot, &fds);
		tv.tv_sec = 0;
		tv.tv_usec = 50000;

		if (select(fdbot + 1, &fds, NULL, NULL, &tv) <= 0)
			goto again;

		if (FD_ISSET(fdtop, &fds)) {
			read (fdtop, &ev, sizeof (struct input_event));
			if (ev.type != 2)
				goto top_bail;
			coords[0][ev.code] = ev.value;
			if (ev.code != 2)
				goto top_bail;
			seen[0]++;
			if ((coords[0][0] > TOLERANCE) ||
			    (coords[0][0] < -TOLERANCE) ||
			    (coords[0][1] > TOLERANCE) ||
			    (coords[0][1] < -TOLERANCE) ||
			    (coords[0][2] > (1000 + TOLERANCE)) ||
			    (coords[0][2] < (1000 - TOLERANCE)))
				fail[0]++;
		}
top_bail:
		if (FD_ISSET(fdbot, &fds)) {
			read (fdbot, &ev, sizeof (struct input_event));
			if (ev.type != 2)
				goto again;
			coords[1][ev.code] = ev.value;
			if (ev.code != 2)
				goto again;
		}

again:
		if (updates_before_exit--)
			continue;

		/* test completed */

		close(fdtop);
		close(fdbot);

		if ((seen[0] < SAMPLES_NEEDED) || (fail[0] > BAD_SAMPLES_ALLOWED))
			i += sprintf(&buf[i], "Top sensor FAIL (%d/%d, %d/%d)\n",
					seen[0], SAMPLES_NEEDED, fail[0],
					BAD_SAMPLES_ALLOWED);
		else
			i += sprintf(&buf[i], "Top sensor PASS\n");

		if ((seen[0] < SAMPLES_NEEDED) || (fail[0] > BAD_SAMPLES_ALLOWED))
			i += sprintf(&buf[i], "Bot sensor FAIL (%d/%d, %d/%d)\n",
					seen[0], SAMPLES_NEEDED, fail[0],
					BAD_SAMPLES_ALLOWED);
		else
			i += sprintf(&buf[i], "Bot sensor PASS\n");

		oltk_view_set_text(view, buf);
		oltk_redraw(oltk);
		return;
	}

err1:
	oltk_view_set_text(view,"Top Sensor Fail");
	oltk_redraw(oltk);
	return;

err2:
	oltk_view_set_text(view,"Bottom Sensor Fail");
	oltk_redraw(oltk);
}



test_t ms_tests[] = {
	/* {
		"Detect",
		do_motion_sensor_test,
		FALSE,
		NULL,
		NULL
	}, */
	{
		"Test",
		do_sensor_test,
		FALSE,
		NULL,
		NULL
	},
	{ NULL }
};

