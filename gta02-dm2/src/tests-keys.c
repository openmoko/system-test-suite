#include "dm2.h"
#include "event-names.h"


static void key_press(char *key )
{
	int fd, rd;
	struct input_event ev;
	char buf[BUFSIZ];
	int count = 8;
	fd_set fds;
	struct timeval tv;
	const char *description;

	/* Confirm device initialized */
	if (access(key, R_OK)) {
		oltk_view_set_text(view, "Fail\n");
		oltk_redraw(oltk);
		return;
	}

	if ((fd = open (key, O_RDONLY)) < 0)
	{
		perror ("evtest");
		exit (1);
	}

	oltk_view_set_text(view, "Waiting For Key Press\n");
	oltk_redraw(oltk);

	rd = 1;
	do {
		FD_ZERO(&fds);
		FD_SET(fd, &fds);
		tv.tv_sec = 0;
		tv.tv_usec = 200000;
		if (select(fd + 1, &fds, NULL, NULL, &tv) > 0)
			read (fd, &ev, sizeof (struct input_event));
		else
			rd = 0;
	} while (rd);

	printf("Waiting For Key Press\n");

	while (count) {
		rd = read (fd, &ev, sizeof(ev));
		printf("read %d / %d\n", rd, sizeof(struct input_event));
		if (rd < (int) sizeof(struct input_event)) {
			printf("yyy\n");
			perror("\nevtest: error reading");
			exit(1);
		}

		if (!ev.type)
			continue;

		switch (ev.value) {
		case 0:
			description = "UP";
			break;
		case 1:
			description = "DOWN";
			break;
		case 2:
			description = "?";
			break;
		}
		
		snprintf(buf, BUFSIZ,
				"Key Press %s!! \n(type %d (%s), "
				"code %d (%s), value %d)\n", description, ev.type,
				events[ev.type] ? events[ev.type] : "?",
				ev.code, names[ev.type] ?
				(names[ev.type][ev.code] ?
				names[ev.type][ev.code] : "?") : "?",
				ev.value);


		printf("%s", buf);

		oltk_view_set_text(view, buf);
		oltk_redraw(oltk);

		count--;
	}
}

static void key911_test(void)
{
	key_press(KEY_911);
}

static void do_key911_test(void)
{
	do_fork(key911_test);
	countdown(KEY_TEST_TIME , FALSE);
}

static void keypower_test(void)
{
	key_press(POWER_KEY);
}

static void do_keypower_test(void)
{
	do_fork(keypower_test);
	countdown(KEY_TEST_TIME , FALSE);
}


test_t key_tests[] = {
	{
		"911 Key",
		do_key911_test,
		FALSE,
		NULL,
		NULL
	},
	{
		"Power Key",
		do_keypower_test,
		FALSE,
		NULL,
		NULL
	},
	{ NULL }
};
