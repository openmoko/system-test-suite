#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <termios.h>
#include <time.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <linux/input.h>

#include "oltk/oltk.h"
#include "dm2_Network.h"
#include "gps.h"

/* including DM_SW_VERSION */
#include "gta02_dm2_version.h"

#define BUTTON_SIZE 120
#define BUTTON_MARGIN 0
#define BUTTON_OFFSET 0

#define DEBUG

#ifndef BUFSIZ
#define BUFSIZ 1024
#endif

#define LCM_TEST_TIME 2*1000
#define LED_TEST_TIME 2
#define BRIGHTNESS_TEST_TIME (LED_TEST_TIME)

#define RTC_TEST_TIME 5
#define KEY_TEST_TIME (RTC_TEST_TIME)
#define VIBRATOR_TEST_TIME 3 /*(KEY_TEST_TIME) */

#define GPS_TEST_TIME 75 /* 45 */
#define GPS_SNTEST_TIME 25

#define BT_ON_TEST_TIME 2
#define BT_TEST_TIME 20

#define WIFI_ON_TEST_TIME (BT_ON_TEST_TIME)
#define WIFI_TEST_TIME 20

#define GSM_TEST_TIME 2
#define GSM_DIAL_TIME 45

#define AUDIO_PLAY_TIME  3
#define AUDIO_TEST_TIME  6

#define PASS (TRUE)
#define FAIL (FALSE)

#define SN_Path ("/mnt/ram/sn")

#define DEFAULTMYDIR "/tmp"

#define Log_Path (DEFAULTMYDIR"/log")

#define BT_MAC_PATH (DEFAULTMYDIR"/bt_mac")
#define GSM_MAC_PATH (DEFAULTMYDIR"/gsm_mac")
#define WIFI_MAC_PATH (DEFAULTMYDIR"/wifi_mac")

#define BT_LOG_PATH (DEFAULTMYDIR"/bt_log")
#define WIFI_LOG_PATH (DEFAULTMYDIR"/wifi_log")

#define TOUCH_PATH "/etc/pointercal"

#define GPS_DEVICE "/dev/ttySAC1"
#define GSM_DEVICE "/dev/ttySAC0"


#define BATTERY_VOLTAGE "/sys/bus/i2c/drivers/pcf50633/0-0073/battvolt"

#define RED_LED_DEVICE "/sys/class/leds/gta02-aux\\:red/brightness"
#define ORANGE_LED_DEVICE "/sys/class/leds/gta02-power\\:orange/brightness"
#define BLUE_LED_DEVICE "/sys/class/leds/gta02-power\\:blue/brightness"

#define VIBRATOR_DEVICE "/sys/class/leds/neo1973\:vibrator/brightness"

#define BRIGHTNESS_DEVICE "/sys/class/backlight/pcf50633-bl/brightness"

#define KEY_911 "/dev/input/event0"
#define POWER_KEY "/dev/input/event4"

#define GPS_POWER "/sys/bus/platform/drivers/neo1973-pm-gps/neo1973-pm-gps.0/pwron"
#define GSM_POWER "/sys/bus/platform/devices/neo1973-pm-gsm.0/power_on"
#define GSM_DL "/sys/bus/platform/devices/neo1973-pm-gsm.0/download1"
#define BT_POWER "/sys/bus/platform/devices/neo1973-pm-bt.0/power_on"
#define BT_RESET "/sys/bus/platform/devices/neo1973-pm-bt.0/reset"
#define WIFI_POWER (BT_POWER)

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)

enum {
	BUTTON_PASS = 0,
	BUTTON_FAIL,
	//BUTTON_LOG,
	BUTTON_QUIT,
	BUTTON_YES,
	BUTTON_NO,
	N_BUTTONS
};


enum{
	SPEAKER=0,
	RECEIVER,
	EARPHONE,
	LOOPBACK_SPEAKER,
	LOOPBACK_RECEIVER,
	LOOPBACK_EARPHONE,
	LOOPBACK_EARMIC_SPEAKER,
	GSM_RECEIVER,
	MAX
};

enum {
	BATTERY = 0,
	DC
};


/*
 * all these externs are coming from dm2.c
 */

typedef struct _test_t {
	const char *name;
	void (*func)(void);
	int result;
	char *log;
	struct oltk_button *button;
} test_t;

extern test_t version_tests[];
extern test_t rtc_tests[];
extern test_t pmu_tests[];
extern test_t mac_address_tests[];
extern test_t vibrator_tests[];
extern test_t led_tests[];
extern test_t audio_tests[];
extern test_t lcm_tests[];
extern test_t ms_tests[];
extern test_t sn_tests[];
extern test_t key_tests[];
extern test_t gps_tests[];
extern test_t bt_tests[];
extern test_t wifi_tests[];
extern test_t gsm_tests[];
extern test_t log_tests[];
extern test_t log_tests_2[];

typedef struct _test_suite {
	const char *name;
	const char *desc;
	test_t *tests;
} test_suite;

extern test_suite suites[];

extern struct oltk *oltk;
extern struct oltk_button *buttons[N_BUTTONS];
extern struct oltk_popup *popup;
extern struct oltk_view *view;

extern int xres, yres;
extern int run;
extern int fixed;
extern int resu;

extern struct termios tio;

extern int active_suite;
extern int active_test;
extern int n_suites;


int do_fork(void  *func);
int countdown(int sec, int avaiable);
int read_log(char *path, char *buf, int size);
int set_data(const char* device ,const char* data);
int countdown_statusfile(int sec, int avaiable, const char *statusfile);

