#include "dm2.h"
#include "event-names.h"


static void key_press(char *key )
{
	int fd, rd, i, j, k;
	struct input_event ev[64];
	int version;
	unsigned short id[4];
	unsigned long bit[EV_MAX][NBITS (KEY_MAX)];
	char name[256] = "Unknown";
	int abs[5];
	char buf[BUFSIZ];
	int count = 2;
	fd_set fds;
	struct timeval tv;

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

	ioctl(fd, EVIOCGVERSION, &version);
	printf("Input driver version is %d.%d.%d\n",
			version >> 16, (version >> 8) & 0xff, version & 0xff);

	ioctl(fd, EVIOCGID, id);
	printf("Input device ID: bus 0x%x vendor 0x%x "
	       "product 0x%x version 0x%x\n",
	       id[ID_BUS], id[ID_VENDOR], id[ID_PRODUCT], id[ID_VERSION]);

	ioctl(fd, EVIOCGNAME (sizeof (name)), name);
	printf("Input device name: \"%s\"\n", name);

	memset(bit, 0, sizeof (bit));
	ioctl(fd, EVIOCGBIT (0, EV_MAX), bit[0]);
	printf("Supported events:\n");

	for (i = 0; i < EV_MAX; i++) {
		if (!test_bit(i, bit[0]))
			continue;

		printf("  Event type %d (%s)\n", i,
						 events[i] ? events[i] : "?");
		ioctl(fd, EVIOCGBIT (i, KEY_MAX), bit[i]);
		for (j = 0; j < KEY_MAX; j++) {
			if (!test_bit(j, bit[i]))
				continue;
			printf("    Event code %d (%s)\n", j, names[i] ?
				(names[i][j] ? names[i][j] : "?") : "?");
			if (i != EV_ABS)
				continue;
			ioctl(fd, EVIOCGABS (j), abs);
			for (k = 0; k < 5; k++)
				if ((k < 3) || abs[k])
					printf("      %s %6d\n",
							     absval[k], abs[k]);
		}
	}

	oltk_view_set_text(view, "Waiting For Key Press\n");
	oltk_redraw(oltk);

	//	read (fd, ev, sizeof (ev));

	rd = 1;
	do {
		FD_ZERO(&fds);
		FD_SET(fd, &fds);
		tv.tv_sec = 0;
		tv.tv_usec = 200000;
		if (select(fd + 1, &fds, NULL, NULL, &tv) > 0)
			read (fd, ev, sizeof (struct input_event));
		else
			rd = 0;
	} while (rd);


	close(fd);

	if ((fd = open (key, O_RDONLY)) < 0) {
		perror ("evtest");
		exit (1);
	}

	printf("Waiting For Key Press\n");

	while (count) {
		rd = read (fd, ev, sizeof(ev));
		printf("read %d / %d\n", rd, sizeof(struct input_event));
		if (rd < (int) sizeof(struct input_event)) {
			printf("yyy\n");
			perror("\nevtest: error reading");
			exit(1);
		}

		for (i = 0; i < rd / sizeof(struct input_event); i++) {
			snprintf(buf, BUFSIZ,
				 "Key Press Down!! \n(type %d (%s), "
				 "code %d (%s), value %d)\n", ev[i].type,
				 events[ev[i].type] ? events[ev[i].type] : "?",
				 ev[i].code, names[ev[i].type] ?
				  (names[ev[i].type][ev[i].code] ?
				   names[ev[i].type][ev[i].code] : "?") : "?",
				 ev[i].value);
			printf("%s", buf);

			oltk_view_set_text(view, buf);
			oltk_redraw(oltk);
		}
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
