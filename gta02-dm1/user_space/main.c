#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <strings.h> 	/* for rindex() */

#include "dm_init.h"

#if 0
/* BASEBAND COMPONENT */
enum {
	BACKLIGHT = 0x00,
	LED,
	KEY,
	RTC,
	VIBRATOR,
	NORFLASH,
	SENSOR,
	SDCARD,
	TOUCHPANEL,
	LCD,
	AUDIO
};

/* GPS COMPONENT */
enum {
	GPS = 0xF0
};

/* GSM COMPONENT */
enum {
	GSM = 0xE0
};

/* BLUETOOTH COMPONENT */
enum {
	BLUETOOTH = 0xD0
};

/* WIRELESS COMPONENT */
enum {
	WIRELESS = 0xC0
};
#endif

#define CMD_BUF_SIZE 64

int main(int argc, char **argv)
{
	static char cmd_string[CMD_BUF_SIZE];
	char *cmd_ptr;

	/* Busybox-like parser */
	if (argc < 2) {
	//	cmd_ptr = rindex(argv[0], '/');
		cmd_ptr = strchr(argv[0], '/');
		if (cmd_ptr == NULL)
			strncpy(cmd_string, argv[0], CMD_BUF_SIZE);
		else {
			cmd_ptr++;
			strncpy(cmd_string, cmd_ptr, CMD_BUF_SIZE);
		}
		argv[1] = cmd_string;
	}

	if (!strcmp(argv[1], "BACKLIGHT")) {
		printf("[BACKLIGHT] start test\n");
		backlight_test();
		printf("[BACKLIGHT] end test\n");
	}

	if (!strcmp(argv[1], "LED")) {
		printf("[LED] start test\n");
		led_test();
		printf("[LED] end test\n");
	}

        if (!strcmp(argv[1], "KEY911")) {
                printf("[KEY911] start test\n");
                key911_test();
                printf("[KEY911] end test\n");
        }

        if (!strcmp(argv[1], "KEYPOWER")) {
                printf("[KEYPOWER] start test\n");
                keypower_test();
                printf("[KEYPOWER] end test\n");
        }

	if (!strcmp(argv[1], "RTC")) {
		printf("[RTC] start test\n");
		rtc_test();
		printf("[RTC] end test\n");
	}

	if (!strcmp(argv[1], "VIBRATOR")) {
		printf("[VIBRATOR] start test\n");
		vibrator_test();
		printf("[VIBRATOR] end test\n");
	}

	if (!strcmp(argv[1], "NORFLASH")) {
		printf("[NORFLASH] start test\n");
		norflash_test();
		printf("[NORFLASH] end test\n");
	}

	if (!strcmp(argv[1], "SENSOR")) {
		printf("[SENSOR] start test\n");
		sensor_test();
		printf("[SENSOR] end test\n");
	}

	if (!strcmp(argv[1], "SDCARD")) {
		printf("[SDCARD] start test\n");
		sdcard_test();
		printf("[SDCARD] end test\n");
	}

	if (!strcmp(argv[1], "TOUCHPANEL")) {
		printf("[TOUCHPANEL] start test\n");
		touchpanel_test();
		printf("[TOUCHPANEL] end test\n");
	}

	if (!strcmp(argv[1], "LCD")) {
		printf("[LCD] start test\n");
		lcd_test();
		printf("[LCD] end test\n");
	}

	if (!strcmp(argv[1], "AUDIO")) {
		printf("[AUDIO] start test\n");
		audio_test();
		printf("[AUDIO] end test\n");
	}

	if (!strcmp(argv[1], "GPS")) {
		printf("[GPS] start test\n");
		gps_test();
		printf("[GPS] end test\n");
	}

	if (!strcmp(argv[1], "GSM")) {
		printf("[GSM] start test\n");
		gsm_test();
		printf("[GSM] end test\n");
	}

	if (!strcmp(argv[1], "DIAL")) {
		printf("[GSM] start dial test\n");
		
		system("libgsmd-tool -m shell</home/default/dialout");

		printf("[GSM] end dial test\n");
	}
	if (!strcmp(argv[1], "BLUETOOTH")) {
		printf("[BLUETOOTH] start test\n");
		bluetooth_test();
		printf("[BLUETOOTH] end test\n");
	}

	if (!strcmp(argv[1], "WIRELESS")) {
		printf("[WIRELESS] start test\n");
		wireless_test();
		printf("[WIRELESS] end test\n");
	}

	if (!strcmp(argv[1], "GRAPHICS")) {
		printf("[GRAPHICS] start test\n");
		graphics_test();
		printf("[GRAPHICS] end test\n");
	}

	if (!strcmp(argv[1], "BATTERY")) {
		printf("[BATTERY] start test\n");
		battery_test();
		printf("[BATTERY] end test\n");
	}

	if (!strcmp(argv[1], "version")) {
		printf("built: %s, %s. by: cdplayer12\n",
			__DATE__, __TIME__);
	}

	return 0;
}
