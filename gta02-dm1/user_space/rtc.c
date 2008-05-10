#include "dm_init.h"

void rtc_test(void)
{
	//int     fd, fd1, fd2, fd3, i;
	//int     rc = 0, rc1 = 0;
	//char    date[15], time[15];

	printf("[RTC] write : ");
	fflush(stdout);
	sleep(2);
	system("cat /sys/class/rtc/rtc0/settime");
	printf("[RTC] read : ");
	fflush(stdout);
	sleep(2);
	system("cat /sys/class/rtc/rtc0/readtime");

}

