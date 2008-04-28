#include "dm2.h"

/* from dm2.c */
extern void do_gsm_log_on_test(void);

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



test_t log_tests[] = {
	{
		"Suspend",
		do_suspend_test,
		FALSE,
		NULL,
		NULL
	},
	{
		"ON",
		do_gsm_log_on_test,
		FALSE,
		NULL,
		NULL
	},
	{ NULL }
};

test_t log_tests_2[] = {
        {
		"ON",
		do_gsm_log_on_test,
		FALSE,
		NULL,
		NULL
	},
	{
		"DL Final Image",
		do_dl_finalimage_test,
		FALSE,
		NULL,
		NULL
	},
        { NULL }
};

