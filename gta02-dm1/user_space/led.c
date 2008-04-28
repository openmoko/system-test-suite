#include "dm_init.h"

void led_test(void)
{
	/* read backlight info */
	int	fd, fd1, fd2;
	int	rc = 0;
	char	rv_buf[16];

	printf("[led-aux-bar] turn on \n");
	fd      = open("/sys/class/leds/gta02-aux:red/brightness", O_RDWR);
	fd1     = open("/sys/class/leds/gta02-power:blue/brightness", O_RDWR);
	fd2     = open("/sys/class/leds/gta02-power:orange/brightness", O_RDWR);

        /* error check */
	if (fd == -1) {
		printf("[led-aux-bar] error\n");
	}

#if 0
        /* read */
	rc = read(fd, rv_buf, sizeof(rv_buf));

        rv_buf[rc] = '\0';
	        
	printf("[led1] : %s \n", rv_buf);
#endif
	/* read */
	rc = read(fd, rv_buf, sizeof(rv_buf));
	rv_buf[rc] = '\0';
	printf("[led-aux-bar] brightness : %s\n", rv_buf);

	/* write */
	rc = write(fd, "255", 3);

	sleep(1);

	rc = write(fd, "0", 1);

	rc = write(fd1, "255", 3);

	sleep(1);

	rc = write(fd1, "0", 1);

	rc = write(fd2, "255", 3);

	sleep(1);

	rc = write(fd2, "0", 1);


        /* error check */
	if (rc == -1) {
		printf("[led-aux-bar] error 1\n");
		close(fd);
		exit(-1);
	}

	/* close file */
	close(fd);
	close(fd1);
	close(fd2);

}
