#include "dm_init.h"

void rtc_test(void)
{
	int     fd, fd1, fd2, fd3, i;
	int     rc = 0, rc1 = 0;
	char    date[15], time[15];

#if 0
	fd      = open("/sys/class/rtc/rtc0/setdate", O_RDONLY);
	fd1      = open("/sys/class/rtc/rtc0/settime", O_RDONLY);
        fd2      = open("/sys/class/rtc/rtc0/date", O_RDONLY);
        fd3      = open("/sys/class/rtc/rtc0/time", O_RDONLY);

	/* error check */
	if (fd == -1 || fd1 == -1) {
		printf("[RTC] write error\n");
		return;
	}

	printf("[RTC] write : ");
	rc = read(fd, date, sizeof(date));
	date[rc] = '\0';

	/* error check */
	if (rc == -1) {
		printf("error\n");
		close(fd);
		exit(-1);
	}

        printf("%s ", date);

        rc1 = read(fd1, time, sizeof(time));
        time[rc1] = '\0';

        /* error check */
        if (rc1 == -1) {
                printf("error\n");
                close(fd1);
                exit(-1);
        }

        printf("%s\n", time);

	sleep(2);

        /* error check */
        if (fd2 == -1 || fd3 == -1) {
                printf("[RTC] read error\n");
                return;
        }

        printf("[RTC] read : ");
        rc = read(fd2, date, sizeof(date));
        date[rc] = '\0';

        /* error check */
        if (rc == -1) {
                printf("error\n");
                close(fd2);
                exit(-1);
        }

        printf("%s ", date);

        rc1 = read(fd3, time, sizeof(time));
        time[rc1] = '\0';

        /* error check */
        if (rc1 == -1) {
                printf("error\n");
                close(fd3);
                exit(-1);
        }

	printf("%s\n", time);

	/* close file */
	close(fd);
	close(fd1);
	close(fd2);
	close(fd3);
#endif

	printf("[RTC] write : ");
	fflush(stdout);
	sleep(2);
	system("cat /sys/class/rtc/rtc0/settime");
	printf("[RTC] read : ");
	fflush(stdout);
	sleep(2);
	system("cat /sys/class/rtc/rtc0/readtime");

}

