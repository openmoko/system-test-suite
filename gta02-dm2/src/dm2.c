/* dm2.c
 *
 * (C) 2007 by OpenMoko, Inc.
 * Written by Nod Huang <nod_huang@oepnmoko.com> 
 *  Modified by Miles Hsieh <miles_hsieh@openmoko.com>
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "dm2.h"


struct oltk *oltk;
struct oltk_button *buttons[N_BUTTONS];
struct oltk_popup *popup;
struct oltk_view *view;

int xres, yres;
int run;
int fd;
int fixed = 0;
int resu = 0;

struct termios tio;

test_suite suites[] = {
	{ "Menu",	"Press `Pass' to Test!!\n",		 version_tests },
	{ "LCM", 	"Check the LCM!!\n",		 	 lcm_tests },
	{ "BlueTooth", 	"Find out the BT status!!\n",	 	 bt_tests },
	{ "Motion",     "Check the Motion sensor!!\n",           ms_tests },
	{ "Mac",        "Check the Mac Address!!\n",             mac_address_tests },
	{ "Audio",      "Listening Carefully!!\n",               audio_tests },
	{ "PMU",	"Check Voltage!!\n",		 	 pmu_tests },
	{ "DM1.5",     	"Save DM1.5 log to PC\n",     		 log_tests },
	{ "Key", 	"Check the key!!\n", 		 	 key_tests },
	//{ "Led",	"Check LED!!\n",		 	 led_tests },
	{ "Vibrator", 	"Vibrator will turn on 3 secs!!\n",	 vibrator_tests },
	{ "RTC",	"Get the RTC information!!\n",	 	 rtc_tests },
	{ "GPS",	"Find out the Fixed time!!\n",	 	 gps_tests },		
	{ "GSM ",     	"Check the GSM status\n",     	 	 gsm_tests },
	{ "WiFi",	"Check the Wifi status!!\n",    	 wifi_tests },
	{ "CN",    	"Check the  GPS CN!!\n", 	 	 cn_tests },
	{ "DM2",     	"Save DM2 log to PC\n",     		 log_tests_2 },

};

int active_suite = -1;
int active_test = -1;
int n_suites = sizeof(suites) / sizeof(test_suite);

static void sig(int sig)
{
	fflush(stderr);
	printf("signal %d caught\n", sig);
	fflush(stdout);

	oltk_abort(oltk);
	run = 0;
}

/*Purpose: internal process communication Using pipe(2) */

static int do_pipe(char *string, const char *execution)
{
	/*  Define gints arrays to ref pipes */
	int	stdin_pipe[2];
	int	stdout_pipe[2];
	int	stderr_pipe[2];

	/*  Define buffs to copy your reads from the pipes to */
	char buffer[BUFSIZ+1];
	char buffer_err[BUFSIZ+1];

	/* Def some vars to hold your results */
	int fork_result;
	int data_processed;
	int data_processed_err;

	/* Clear Your Buffers */
	memset(buffer, 0, sizeof(buffer));
	memset(buffer_err, 0, sizeof(buffer_err));

	/* Do the fork and pipe - watch closely here */

	/* Create pipes to read from and write too */
	if ((pipe(stdin_pipe)) || (pipe(stdout_pipe)) || (pipe(stderr_pipe)))
		return TRUE;
	
	/* Perform the fork */
	fork_result = fork();

	/* fork attempt was not successful */
	if (fork_result == -1)
	{
		fprintf(stderr, "do_fork Failure\n");
		exit(EXIT_FAILURE);
		return FALSE;
	}

	/*  We're in the child process! */
	if (fork_result == 0) {
		/* Close the Child process' STDIN */
		close(0);

		/* Duplicate the Child's STDIN to the
			* stdin_pipe file descriptor */
		dup(stdin_pipe[0]);

		/* Close the read and write to for the pipe for
			* the child.  The child will now only be able to
			* read from it's STDIN (which is our pipe). */
		close(stdin_pipe[0]);
		close(stdin_pipe[1]);

		/* Close the Child process' STDOUT */
		close(1);
		dup(stdout_pipe[1]);
		close(stdout_pipe[0]);
		close(stdout_pipe[1]);

		/* Close the Child process' STDERR */
		close(2);
		dup(stderr_pipe[1]);
		close(stderr_pipe[0]);
		close(stderr_pipe[1]);

		/*  Make the exec call to run the script. */
		system(execution);

		/*  If console didn't take over the exec call failed. */
		exit(EXIT_FAILURE);
	}

	/* Close STDIN for read & write and close
		* STDERR for write */
	close(stdin_pipe[0]);
	close(stdin_pipe[1]);
	close(stderr_pipe[1]);

	while (1) {
		data_processed_err = read(stderr_pipe[0],
						buffer_err, BUFSIZ);
		if (!data_processed_err)
			break;
	}
	/* Close the read end of STDERR */
	close(stderr_pipe[0]);
	/* Close the write end of STDOUT */
	close(stdout_pipe[1]);

	while(1)
	{
		/* Read BUFSIZ of STDOUT data */
		data_processed=read(stdout_pipe[0],buffer,BUFSIZ);

		/* return the string */
		if (strlen(buffer) && !data_processed)
			strcpy(string,buffer);
			//printf("pipe:%s\n",buffer);

		/* Break this loop when we're read
			* all of the STDOUT data */
		if (data_processed == 0)
			break;
	}

	/* Close STDOUT for reading */
	close(stdout_pipe[0]);

	return TRUE;
}

/* Purpose: excute another process Using fork(2) */

int do_fork(void  *func)
{
	/* Def some vars to hold your results */
	int fork_result = fork();

	if (fork_result)
		return TRUE;

	/* fork attempt was not successful */
	if (fork_result == -1) {
		fprintf(stderr, "do_fork Failure\n");
		exit(EXIT_FAILURE);
		return FALSE;
	}

	/*  We're in the child process! */
	/*  Make the exec call to run the script. */
	void (*fp) (void) = func;
	fp();

	/*  If console didn't take over the exec call failed. */
	exit(EXIT_FAILURE);
}

static int write_log(char *path, char *name, char *param)
{
	FILE * fp;

	if (!name[0])
		return FALSE;

	fp = fopen(path, param);
	if (fp == NULL)
		return FALSE;

	fwrite(name, 1, strlen(name), fp);
	fclose(fp);

	return TRUE;
}

/*Purpose: read data from file */

int read_log(char *path, char *buf, int size)
{ 
	FILE * fp = fopen(path,"r");
	if (fp == NULL)
		return FALSE;
	
	memset(buf, 0, size);
	fread(buf, 1, size, fp);
	fclose(fp);

	return TRUE;
}

void do_gsm_log_on_test(void)
{
	char tmp[BUFSIZ];
	char buf[BUFSIZ];

	memset(buf, 0, sizeof(buf));

	//system("cp log tmp/");//"cp tmp/log ../");
	system("mkdir tmp");
	system("cp log tmp/");

	snprintf(buf, BUFSIZ, "SW Version : %s \n", DM_SW_VERSION);

	strcat(buf, "\nS/N : ");

	memset(tmp, 0, sizeof(tmp));
	read_log(SN_Path, tmp, BUFSIZ);

	strcat(buf, tmp);

	strcat(buf, "\nWeb Server ready!!\n");

	do_fork(Start_Server);

	oltk_view_set_text(view, buf);
	oltk_redraw(oltk);

	system("echo 1 > /sys/bus/platform/devices/neo1973-pm-gsm.0/download");
}


void on_log(struct oltk_button *b, void *data)
{	 
	do_gsm_log_on_test();
}

static void save_item_result(int i,int j)
{
	char buf[BUFSIZ];

	memset(buf, 0, sizeof(buf));

	write_log(Log_Path, "\n----------Start Test----------\n","a+");   

	snprintf(buf, BUFSIZ, "\nModule :%s\n", suites[i].name);   
	write_log(Log_Path, buf, "a+");

	test_t *tests = suites[i].tests;

	if (tests) {
		snprintf(buf, BUFSIZ, "\nItem Name: %s\nResult : %s\n",
				      tests[j].name,
				      tests[j].result ? "Pass" : "Fail");
		write_log(Log_Path,buf, "a+");

		if (tests[j].log) {
			printf("%s\n", tests[j].log);
			snprintf(buf, BUFSIZ, "\nLog : %s\n", tests[j].log);

			write_log(Log_Path,buf, "a+");
		}		

	}

	write_log(Log_Path, "\nEOF\n", "a+");
}

void next_test(struct oltk_popup *pop)
{
	test_t *tests = suites[active_suite].tests;

	if (tests)
		if (active_test > -1)
			save_item_result(active_suite,active_test);

	/* last test in active test suite */
	while (!tests || !tests[active_test + 1].name)
	{
		if (!oltk_popup_set_selected(pop, active_suite + 1))
			on_log((struct oltk_button *) popup, NULL);
		return;
		/* FIXME: Huh?  This is unreachable code */
		tests = suites[active_suite].tests;
	}

	active_test++;

	//printf("test (%d, %d) begin\n", active_suite, active_test);
	oltk_button_click(tests[active_test].button);
}


static void on_pass(struct oltk_button *b, void *data)
{
	test_t *tests = suites[active_suite].tests;

	if (active_test>-1 && tests)
		tests[active_test].result = PASS;

	//printf("test (%d, %d) passed\n", active_suite, active_test);
	next_test(data);
}

static void on_fail(struct oltk_button *b, void *data)
{
	test_t *tests = suites[active_suite].tests;

	if (tests)
		if (active_test > -1)
			tests[active_test].result = FAIL;

	//printf("test (%d, %d) failed\n", active_suite, active_test);
	next_test(data);
}

int countdown(int sec, int avaiable)
{
	while (sec)
	{
		sec--;

		if (avaiable) {	
			char buf[10] = { '\0' };

			snprintf(buf, 10, "%5d sec", sec);
			oltk_view_set_text(view, buf);
			oltk_redraw(oltk);	    
		}
		oltk_msleep(oltk, 1000);			   
	}
	return TRUE;	
}

int set_data(const char* device ,const char* data)
{
	FILE *fp = NULL;

	/* Confirm device initialized */
	if (access(device, R_OK)) {
		oltk_view_set_text(view, "Fail\n");
		oltk_redraw(oltk);
		return FALSE;
	}

	if ((fp = fopen(device, "w"))) 
	{
		fwrite(data, sizeof(char), strlen(data), fp);
		if (atoi(data)) {
			oltk_view_set_text(view, "ON");
			//printf("ON");	
		}
		else{
			oltk_view_set_text(view, "OFF");
			//printf("OFF");
		}
		oltk_redraw(oltk);
		fclose(fp);

		return TRUE; 
	} else {
		oltk_view_set_text(view, "Fail");
		oltk_redraw(oltk);
		return FALSE;
	}

}


static void dl_finalimg_test(void)
{
	system("dl_finalimg");
}

static void do_dl_finalimage_test(void)
{
	do_fork(dl_finalimg_test);
/*
        countdown(90,TRUE);

        if (access("/tmp/kernel-done",R_OK) != 0) {
                printf("kernel:Fail\n");
                oltk_view_set_text(view, "Kernel Fail\n");
                oltk_redraw(oltk);
        }
        else {
		if (access("/tmp/rootfs-done",R_OK) != 0) {
                	printf("rootfs:Fail\n");
                	oltk_view_set_text(view, "Kernel Pass\nrootfs Fail\n");
                	oltk_redraw(oltk);
	        }
        	else {
                	printf("rootfs:Pass\n");
                	oltk_view_set_text(view, "Kernel Pass\nrootfs Pass\n");
                	oltk_redraw(oltk);
		}
        }
*/
        countdown(100, TRUE);

        if (access("/tmp/uboot-done",R_OK)) {
                printf("uboot:Fail\n");
                oltk_view_set_text(view, "uboot Fail\n");
                oltk_redraw(oltk);
		return;
        }
	if (access("/tmp/env-done",R_OK)) {
		printf("env:Fail\n");
		oltk_view_set_text(view, "uboot Pass\nenv Fail\n");
		oltk_redraw(oltk);
		return;
	}
	if (access("/tmp/splash-done",R_OK) != 0) {
		printf("splash:Fail\n");
		oltk_view_set_text(view, "uboot Pass\nenv Pass\nsplash Fail\n");
		oltk_redraw(oltk);
		return;
	}
	if (access("/tmp/kernel-done",R_OK) != 0) {
		printf("kernel:Fail\n");
		oltk_view_set_text(view, "uboot Pass\nenv Pass\nsplash Pass\n"
					 "Kernel Fail\n");
		oltk_redraw(oltk);
		return;
	}
	if (access("/tmp/rootfs-done",R_OK) != 0) {
		printf("rootfs:Fail\n");
		oltk_view_set_text(view, "uboot Pass\nenv Pass\nsplash Pass\n"
					 "Kernel Pass\nrootfs Fail\n");
		oltk_redraw(oltk);
		return;
	}
	printf("rootfs:Pass\n");
	oltk_view_set_text(view, "uboot Pass\nenv Pass\nsplash Pass\n"
				 "Kernel Pass\nrootfs Pass\n");
	oltk_redraw(oltk);
}


int brightness_test(int level)
{
	switch(level) {
	case 1:
		return set_data(BRIGHTNESS_DEVICE,"0");

	case 2:
		return set_data(BRIGHTNESS_DEVICE,"15");

	case 3:
		return set_data(BRIGHTNESS_DEVICE,"39");

	case MAX:
		return set_data(BRIGHTNESS_DEVICE,"63");
	}
	return FALSE;
}


static int get_voltage(const char* device ,int type)
{
	FILE *fp = NULL;
	char battery_voltage_value[8];
	int i;
	float total = 0, average = 0;
	char buffer[8];

	/* Confirm device initialized */
	if (access(device, R_OK))
		goto err;

	for (i = 0; i< 10; i++)
		if ((fp = fopen(device, "r"))) {
			if (fread(battery_voltage_value, sizeof(char), 5, fp))
				total += atoi(battery_voltage_value);
			else {
				fclose(fp);
				break;
			}
		} else
			goto err;

	fclose(fp);

	if (i == 10) {
		average = total / 10;
		if (total) {
			if (type != BATTERY)
				average = ((average* 5 /875) - 0.22);

			printf("Battery:%2f mV\n",average);
			sprintf(buffer,"%2f mV\n",average);
			oltk_view_set_text(view, buffer);
		} else
			oltk_view_set_text(view, "0 V");

		return 1;

	} else {
err:
		oltk_view_set_text(view, "Fail\n");
		return 0;
	}
}

static void do_battery_test(void)
{
	get_voltage(BATTERY_VOLTAGE,BATTERY);
	oltk_redraw(oltk);

}

static void do_ac_test(void)
{
	get_voltage(BATTERY_VOLTAGE,DC);
	oltk_redraw(oltk);

}

static void do_suspend_test(void)
{
	oltk_view_set_text(view, "Please Wait 10 seconds to wake up");
	oltk_redraw(oltk);
	brightness_test(1);

	// system("apm -s");
	// system("echo mem > /sys/power/state");
	system("cp /home/root/suspend-workaround.sh /tmp/");
	system("./suspend-workaround.sh");

	// oltk_view_set_text(view, "Wake Up Success");
	oltk_view_set_text(view, "Not yet");
	oltk_redraw(oltk);
	brightness_test(MAX);
}


int set_uart_bautrate(char *device, char *speed)
{
	int ret, rate, b_rate = B0;

	/* Confirm device initialized */
	if (access(device, R_OK) != 0) {
		oltk_view_set_text(view, "Fail\n");
		oltk_redraw(oltk);
		return FALSE;
	}

	fd = open(device, O_RDWR);

	if (fd < 0) {
		perror(device);
		exit(1);
	}

	ret = tcgetattr(fd, &tio);
	if (ret < 0)
		return;	

	ret = cfsetispeed(&tio, B0);
	if (ret < 0)
		return;	

	switch (atoi(speed)) {
	case 115200:
		b_rate = B115200;
		break;
	case 57600:
		b_rate = B57600;
		break;
	case 38400:
		b_rate = B38400;
		break;
	case 19200:
		b_rate = B19200;
		break;
	case 9600:
		b_rate = B9600;
		break;
	case 4800:
		b_rate = B4800;
		break;
	case 2400:
		b_rate = B2400;
		break;
	case 1200:
		b_rate = B1200;
		break;
	default:
		printf ("set_uart_bautrate: Error bad rate '%s'\n", speed);
		exit (1);
	}

	cfsetispeed(&tio, b_rate);

	tio.c_cflag &= ~CRTSCTS;
	tio.c_lflag &= ~ICANON;
	tio.c_lflag &= ~ECHO;

	tcsetattr(fd, TCSANOW, &tio);

	return TRUE;    
}

//------------------------------------------------------------------------------
//
//              gta02
//
//------------------------------------------------------------------------------

static ssize_t bwrite(int fd, const void *buf, size_t count, int delay)
{
	size_t i;

	if (!delay)
		return write(fd, (unsigned char *) buf, count);
	
	for (i = 0; i < count; i++)
	{
		if (write(fd, (unsigned char *) buf + i, 1) != 1)
			return -1;
		usleep(delay * 1000);
	}


	return i;
}

static void gllin_ubx(char cls, char id, const unsigned char *ubx, int size, int delay)
{
	unsigned char header[6], checksum[2];
	int i;

	if (!fd)
		return;

	header[0] = 0xb5;
	header[1] = 0x62;
	header[2] = cls;
	header[3] = id;
	header[4] = size & 0xff;
	header[5] = (size >> 8) & 0xff;

	checksum[0] = checksum[1] = 0;
	for (i = 2; i < 6; i++)
	{
		checksum[0] += header[i];
		checksum[1] += checksum[0];
	}

	for (i = 0; i < size; i++)
	{
		checksum[0] += ubx[i];
		checksum[1] += checksum[0];
	}

	if (bwrite(fd, header, 6, delay) != 6)
		goto fail;

	if (size)
		if (bwrite(fd, (void *) ubx, size, delay) != size)
			goto fail;

	if (bwrite(fd, checksum, 2, delay) != 2)
		goto fail;

#if 0

	printf("ubx sent: ");
	for (i = 0; i < 6; i++)
		printf("%02x ", header[i]);
	if (size)
	{
		for (i = 0; i < size; i++)
			printf("%02x ", ubx[i]);
	}

	printf("%02x %02x\n", checksum[0], checksum[1]);
#endif

	return;

fail:
	printf("ubx failed\n");
}

void gps_reset(char c)
{
	unsigned char ubx[4];
	int bbr;

	switch (c)
	{
		case 'c':
			bbr = 0x7ff;
			break;
		case 'w':
			bbr = 0x1;
			break;
		case 'h':
			bbr = 0x0;
			break;
	}

	ubx[0] = bbr & 0xff;
	ubx[1] = (bbr >> 8) & 0xff;
	ubx[2] = 0x8; /* stop GPS */
	ubx[3] = 0x0;

	gllin_ubx(0x06, 0x04, ubx, 4, 4);

	sleep(2);

	ubx[2] = 0x9; /* start GPS */
	gllin_ubx(0x06, 0x04, ubx, 4, 4);

}

int get_gps_data(char* device, char* buf)
{
	FILE *fp;
	int fixed1 ;
	int len = 0;

	static char temp[64+1];

	fp = fopen(device, "r");

	memset(temp, 0, sizeof(temp));

	if (NULL != fp) {
		len = fread(temp, sizeof(char), 64, fp);

		if (len > 0) {
			const char *agps_nema_data = agps_nmea_process(temp,
								       &fixed1);
			if (agps_nema_data)
				sprintf(buf, agps_nema_data);		
		} 
		else
			sprintf(buf,"[ERROR]Fail to read NEAM 0813 Messages!\n");

		//printf("Translator:%s\n",buf);
		fclose(fp);	  

		return fixed1;

	} else {
		sprintf(buf,"[ERROR]Fail to open device interface!\n");

		//printf("%s",buf);
		return 0;

	}
}


void audio_path(int path)
{
	switch(path) {
	case SPEAKER:
		system("alsactl -f /etc/play_wav_speaker.state restore");
		break;

	case RECEIVER:
		system("alsactl -f /etc/play_wav_receiver.state restore");
		break;

	case EARPHONE:
		system("alsactl -f /etc/play_wav_earphone.state restore");
		break;

	case LOOPBACK_SPEAKER:
		system("alsactl -f /etc/loopback_speaker.state restore");
		break;

	case LOOPBACK_EARPHONE:
		system("alsactl -f /etc/loopback_earphone.state restore");
		break;

	case LOOPBACK_RECEIVER:
		system("alsactl -f /etc/loopback_receiver.state restore");
		break;

	case LOOPBACK_EARMIC_SPEAKER:
		system("alsactl -f /etc/loopback_earmic_speaker.state restore");
		break;

	case GSM_RECEIVER:
		system("alsactl -f /etc/gta02_gsm_receiver.state restore");
		break;

	default:
		break;
	}
}



static void do_sd_on_test(void)
{

	//do_fork(gprs_on_test);
	countdown(GSM_TEST_TIME,TRUE);

	// system("kill -9 `ps | grep pppd | awk '{print $1}'`");

}


static void set_active_test(test_t *t)
{
	test_t *tests = suites[active_suite].tests;
	int i = 0;

	if (!tests) {
		active_test = -1;
		return;
	}

	for (i = 0; tests->name; i++, tests++) {
		if (tests == t) {
			active_test = i;
			break;
		}
	}
}

static void on_test_by_function(struct oltk_button *b, test_t *t)
{
	oltk_redraw(oltk);

	if (t->func)
		t->func();

	set_active_test(t);
}

static void add_test(struct oltk *oltk, test_t *t, int x, int y,
		     int width, int height)
{
	t->button = oltk_button_add(oltk, x, y, width, height);
	oltk_button_set_name(t->button, t->name);

	oltk_button_set_cb(t->button, OLTK_BUTTON_CB_CLICK,
				      on_test_by_function, t);
}

static void on_popup_select(struct oltk_popup *pop, int selected, void *data)
{
	const test_t *t;

	if (selected == active_suite)
		return;

	oltk_view_set_text(view, suites[selected].desc);

	if (active_suite >= 0 && (t = suites[active_suite].tests))
		for (; t->name; t++)
			oltk_button_show(t->button, 0);

	if (t = suites[selected].tests)
		for (; t->name; t++)
			oltk_button_show(t->button, 1);

	active_suite = selected;
	active_test = -1;
}

static void quit_check(struct oltk *oltk)
{
	buttons[BUTTON_YES] = oltk_button_add(oltk, 180,
						    yres / 2 - BUTTON_SIZE,
						    BUTTON_SIZE,
						    BUTTON_SIZE);

	oltk_button_set_name(buttons[BUTTON_YES], "YES");
	oltk_button_show(buttons[BUTTON_YES], 1);

	buttons[BUTTON_NO] = oltk_button_add(oltk, 180 + BUTTON_SIZE,
						   yres/2-BUTTON_SIZE,
						   BUTTON_SIZE, BUTTON_SIZE);

	oltk_button_set_name(buttons[BUTTON_NO], "NO");
	oltk_button_show(buttons[BUTTON_NO], 1);

}
static void on_yes(struct oltk_button *b, void *data)
{
	printf("test (%d, %d) quit\n", active_suite, active_test);

	if (!set_data(GSM_DL,"1")) {
		oltk_view_set_text(view,"Fail");
		oltk_redraw(oltk);
		return;
	}

	run=FALSE;	
}
static void on_no(struct oltk_button *b, void *data)
{
	int i;
	printf("test (%d, %d) quit\n", active_suite, active_test);

	oltk_button_show(buttons[BUTTON_YES], 0);
	oltk_button_show(buttons[BUTTON_NO], 0);

	return;
}
static void on_quit(struct oltk_button *b, void *data)
{
	int i;
	printf("test (%d, %d) quit\n", active_suite, active_test);

	quit_check(oltk);

	oltk_button_set_cb(buttons[BUTTON_YES], OLTK_BUTTON_CB_CLICK,
								  on_yes, oltk);
	oltk_button_set_cb(buttons[BUTTON_NO], OLTK_BUTTON_CB_CLICK,
								   on_no, oltk);

	system("echo 1 > /sys/bus/platform/devices/neo1973-pm-gsm.0/download");
}

static void setup_popup(struct oltk *oltk)
{
	int i;

	for (i = 0; i < n_suites; i++) {
		test_t *t = suites[i].tests;
		int y;

		if (!t)
			continue;

		y = BUTTON_SIZE + BUTTON_MARGIN * 3;
		while (t->name) {
			add_test(oltk, t, BUTTON_MARGIN, y,
					  BUTTON_SIZE, BUTTON_SIZE / 3);
			y += BUTTON_SIZE / 3 + BUTTON_MARGIN;

			t++;
		}
	}

	popup = oltk_popup_add(oltk, 0, 0, BUTTON_SIZE, BUTTON_SIZE, n_suites);
	oltk_popup_set_cb(popup, on_popup_select, NULL);

	for (i = 0; i < n_suites; i++)
		oltk_popup_entry(popup, i, suites[i].name);

	oltk_popup_set_selected(popup, 0);

	oltk_button_set_cb(buttons[BUTTON_PASS], OLTK_BUTTON_CB_CLICK,
								on_pass, popup);
	//oltk_button_set_cb(buttons[BUTTON_LOG], OLTK_BUTTON_CB_CLICK,
	//							on_log, popup);
	oltk_button_set_cb(buttons[BUTTON_QUIT], OLTK_BUTTON_CB_CLICK,
								on_quit, popup);
	oltk_button_set_cb(buttons[BUTTON_FAIL], OLTK_BUTTON_CB_CLICK,
								on_fail, popup);
}

static void setup_check(struct oltk *oltk)
{
	buttons[BUTTON_PASS] = oltk_button_add(oltk,
					       BUTTON_SIZE + BUTTON_MARGIN * 3,
					    yres - BUTTON_SIZE - BUTTON_MARGIN,
					    BUTTON_SIZE, BUTTON_SIZE);

	oltk_button_set_name(buttons[BUTTON_PASS], "Pass");
	oltk_button_show(buttons[BUTTON_PASS], 1);

	buttons[BUTTON_QUIT] = oltk_button_add(oltk, BUTTON_SIZE * 2 +
					       BUTTON_MARGIN * 3 +BUTTON_OFFSET,
					     yres - BUTTON_SIZE - BUTTON_MARGIN,
						      BUTTON_SIZE, BUTTON_SIZE);

	oltk_button_set_name(buttons[BUTTON_QUIT], "Quit");
	oltk_button_show(buttons[BUTTON_QUIT], 1);
	/*
	   buttons[BUTTON_QUIT] = oltk_button_add(oltk, BUTTON_SIZE*3 + BUTTON_MARGIN * 3 +BUTTON_OFFSET*2,
	   yres - BUTTON_SIZE - BUTTON_MARGIN,
	   BUTTON_SIZE, BUTTON_SIZE);

	   oltk_button_set_name(buttons[BUTTON_QUIT], "Quit");
	   oltk_button_show(buttons[BUTTON_QUIT], 1);
	   */
	buttons[BUTTON_FAIL] = oltk_button_add(oltk,
					     xres - BUTTON_SIZE - BUTTON_MARGIN,
					     yres - BUTTON_SIZE - BUTTON_MARGIN,
					     BUTTON_SIZE, BUTTON_SIZE);

	oltk_button_set_name(buttons[BUTTON_FAIL], "Fail");
	oltk_button_show(buttons[BUTTON_FAIL], 1);
}

static int check_factory_info(void)
{
	char buffer[BUFSIZ];
	int result;

	system("mount /dev/mtdblock5 /mnt/ram -r -t ext2");

	if (!read_log(SN_Path,buffer,BUFSIZ))
		return FALSE;

	if (read_log(Log_Path,buffer,BUFSIZ)) {
		result = write_log(Log_Path,
				   "\n----------DM Log File----------\n", "a+");
		return result;
	}

	result = write_log(Log_Path,
				    "\n----------DM Log File----------\n", "w");

	result = write_log(Log_Path,"\nS/N : ","a+");
	result = read_log(SN_Path,buffer,BUFSIZ);
	result = write_log(Log_Path,buffer,"a+");

	return result;
}

static int do_touchpanel_test(void)
{
	system("ts_calibrate");

	if (access(TOUCH_PATH, R_OK) != 0) {
		printf("Touch Panel Calibrate Fail!\n");
		return FALSE;
	}

	return TRUE;
}

int main(int argc, char **argv)
{
	char *dev = NULL;


	signal(SIGSEGV, sig);
	signal(SIGINT, sig);
	signal(SIGTERM, sig);


	if (argc >= 2)
		dev = argv[1];

	system("echo 0 > /sys/bus/platform/devices/neo1973-pm-gsm.0/download");

	if (do_touchpanel_test())
	{
		run = check_factory_info();	
	}

	oltk = oltk_new(dev);
	if (!oltk) {
		printf("oltk did not init\n");
		return 1;
	}

	oltk_get_resolution(oltk, &xres, &yres);

	view = oltk_view_add(oltk, BUTTON_SIZE + BUTTON_MARGIN * 2, 0,
			xres - BUTTON_SIZE - BUTTON_MARGIN * 2,
			yres - BUTTON_SIZE - BUTTON_MARGIN * 2);

	setup_check(oltk);
	setup_popup(oltk);

	oltk_redraw(oltk);

	run = 1;

	while (run) {
		struct oltk_event e;
		int ret = oltk_listen(oltk, &e);

		if (ret < 0)
			break;
		if (!ret)
			continue;

		oltk_feed(oltk, &e);
		oltk_redraw(oltk);
	}

	oltk_free(oltk);

	//	system("umount /mnt/ram ");
	printf("Bye Bye\n");

	system("kill -9 `ps | grep dm2 | awk '{print $1}'`");
	return 0;
}
