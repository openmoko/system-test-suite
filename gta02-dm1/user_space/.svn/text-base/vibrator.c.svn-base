#include "dm_init.h"

void vibrator_test(void)
{
	int     fd;
	int	rc = 0;

	fd      = open("/sys/class/leds/neo1973:vibrator/brightness", O_RDWR);

	/* error check */
        if (fd == -1) {
		printf("[VIBRATOR] error\n");
	}

	rc = write(fd, "255", 3);
	sleep(2);
	rc = write(fd, "0", 1);

        /* error check */
        if (rc == -1) {
                printf("[VIBRATOR] error\n");
                close(fd);
                exit(-1);
        }

        /* close file */
        close(fd);
}
