#include "dm2.h"

static void do_view_version(void)
{
	char buf[BUFSIZ];

	memset(buf, 0, sizeof(buf));
	snprintf(buf, BUFSIZ, "SW Version : %s \n", DM_SW_VERSION);
	oltk_view_set_text(view, buf);
}

static void norflash_test(void)
{
	system("flashnor /home/root/nor.bin-gta02v5-20080305 && "
	       "touch /tmp/nor-done;");

	sleep(1);

        if (access("/tmp/nor-done",R_OK)) {
		printf("nor:Fail\n");
                oltk_view_set_text(view, "Fail\n");
                oltk_redraw(oltk);
	} else {
		printf("nor:Pass\n");
                oltk_view_set_text(view, "Pass");
                oltk_redraw(oltk);
	}
}

static void do_norflash_test(void)
{
        do_fork(norflash_test);
        countdown(1, FALSE);
}

test_t version_tests[] = {
	{
		"Version",
		do_view_version,
	  	FALSE,
		NULL,
		NULL
	},
	{
		"NOR Flash",
		do_norflash_test,
		FALSE,
		NULL,
		NULL
	},
	{ NULL }
};
