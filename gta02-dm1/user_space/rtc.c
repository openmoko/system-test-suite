#include "dm_init.h"
#include <time.h>

void rtc_test(void)
{
	/* output sample:
	 * [RTC] start test
	 * [RTC] write : 2007-06-20  03:10:20
	 * [RTC] read : 2007-06-20  03:10:21
	 * [RTC] end test
	 */
	time_t clock;
	struct tm *tm_data;
	unsigned short loop_i;
	char buff[64];
	char formated_time[2][32] = { "[RTC] write: %Y-%m-\%d \%X",
					"[RTC] read: %Y-%m-\%d \%X" };
	for(loop_i = 0; loop_i <= 1; loop_i++)	{
		memset(buff, 0x0, sizeof(buff));
		time(&clock);
		tm_data = gmtime(&clock);
		strftime(buff, sizeof(buff),formated_time[loop_i], tm_data);
		printf("%s\n", buff);
		if(!loop_i)
			sleep(1);
	}
}

