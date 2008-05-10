#ifndef _DM_INIT_H
#define _DM_INIT_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

void backlight_test(void);
void led_test(void);
void key911_test(void);
void keypower_test(void);
void rtc_test(void);
void vibrator_test(void);
void norflash_test(void);
void sensor_test(void);
void sdcard_test(void);
void touchpanel_test(void);
void lcd_test(void);
void audio_test(void);
void gps_test(void);
void bluetooth_test(void);
void wireless_test(void);
void graphics_test(void);
void battery_test(void);

#define RET_OK  	" => ok\n"
#define RET_FAIL 	" => fail\n"

#endif
