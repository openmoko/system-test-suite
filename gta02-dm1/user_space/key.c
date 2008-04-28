#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <time.h>
#include <linux/input.h>
#include "dm_init.h"

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)

static void key_press(char *key, char *str);

void key911_test(void)
{
	printf("[KEY911] ");
	key_press("/dev/input/event0", "[KEY911]");	
}

void keypower_test(void)
{
	printf("[KEYPOWER] ");
	key_press("/dev/input/event2", "[KEYPOWER]");
}

static void key_press(char *key, char *str)
{
	int fd, rd;
	struct input_event ev[64];
	int version;
	unsigned short id[4];
	unsigned long bit[EV_MAX][NBITS (KEY_MAX)];
	char name[256] = "Unknown";
	int count = 1;

	if ((fd = open (key, O_RDONLY)) < 0)
	{
		perror ("evtest");
		exit (1);
	}

	ioctl(fd, EVIOCGVERSION, &version);
	ioctl(fd, EVIOCGID, id);
	ioctl(fd, EVIOCGNAME (sizeof (name)), name);

	memset(bit, 0, sizeof (bit));
	ioctl(fd, EVIOCGBIT (0, EV_MAX), bit[0]);

	printf("Waiting For Key Press\n");

	while (count)
	{
		rd = read (fd, ev, sizeof (struct input_event) * 64);

		printf("%s ", str);

		if (rd < (int) sizeof (struct input_event))
		{
			printf("\n");
			perror("\nevtest: error reading");
			exit(1);
		}

		printf("Key Press Down!! \n");

		count--;
	}
}

