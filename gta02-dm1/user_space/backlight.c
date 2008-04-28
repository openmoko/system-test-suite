#include "dm_init.h"

void backlight_test(void)
{
	/* read backlight info */
	int	fd;
	int	rc = 0;
	char	rv_buf[16];

	/* open file */
	fd = open("/sys/class/backlight/pcf50633-bl/actual_brightness", O_RDONLY);

	/* error check */
	if (fd == -1) {
		printf("error\n");
	}

	/* read */
	rc = read(fd, rv_buf, sizeof(rv_buf));

	/* error check */
	if (rc == -1) {
		printf("error 1\n");
		close(fd);
		exit(-1);
	}

	rv_buf[rc] = '\0';
	
	printf("[backlight] brightness : %s \n", rv_buf);

	/* close file */
	close(fd);

}
