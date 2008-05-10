#include "dm_init.h"
#include "libts/src/tslib.h"

void touchpanel_test(void)
{
	struct tsdev *ts;
	char *tsdevice=NULL;
	int count=0;

        if( (tsdevice = getenv("TSLIB_TSDEVICE")) != NULL ) {
                ts = ts_open(tsdevice,0);
        } else {
                ts = ts_open("/dev/input/event1", 0);
        }

	if (!ts) {
		printf("ts_open error \n");
		exit(1);
	}

	if (ts_config(ts)) {
		printf("ts_config error \n");
		exit(1);
	}

	while (1) {
		struct ts_sample samp;
		int ret;

		ret = ts_read(ts, &samp, 1);

		if (ret < 0) {
			perror("ts_read");
			exit(1);
		}

		if (ret != 1)
			continue;

		count++;
		printf("%d - %ld.%06ld: %6d %6d %6d\n", count, samp.tv.tv_sec, samp.tv.tv_usec, samp.x, samp.y, samp.pressure);

		if(count == 20)
			break;
	}
}
