#include "dm_init.h"

void bluetooth_test(void)
{
	int     fd, fd1;
	int     rc = 0, rc1 = 0;

	fd      = open("/sys/bus/platform/devices/neo1973-pm-bt.0/power_on", O_RDWR);
	fd1      = open("/sys/bus/platform/devices/neo1973-pm-bt.0/reset", O_RDWR);

	/* error check */
	if (fd == -1 || fd1 == -1) {
		printf("[BLUETOOTH] error\n");
		return;
	}

	/* write */
	rc = write(fd, "0", 1);
	rc1 = write(fd1, "0", 1);

	/* error check */
	if (rc == -1 || rc1 == -1) {
		printf("[BLUETOOTH] error\n");
		close(fd);
		exit(-1);
	}

	sleep(1);

	system("lsusb");
	system("hcitool scan");

	/* close file */
	close(fd);
	close(fd1);
}
