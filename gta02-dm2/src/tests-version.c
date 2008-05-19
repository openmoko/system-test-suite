#include "dm2.h"

static void do_view_version(void)
{
	char buf[BUFSIZ];

	memset(buf, 0, sizeof(buf));
	snprintf(buf, BUFSIZ, "SW Version : %s \n\n"
			"Built: %s, %s\n", DM_SW_VERSION, __DATE__, __TIME__);
	oltk_view_set_text(view, buf);
}

static void norflash_test(void)
{
	
	system("flashnor_check /home/root/nor.bin-gta02v5-* &&"
		"touch /tmp/flashnor_check_done");

        countdown(1, FALSE);
        if (!access("/tmp/flashnor_check_done",R_OK)) {
		printf("nor_check: Pass\n");
                oltk_view_set_text(view, "Pass");
                oltk_redraw(oltk);
		return;
	}
	/* check fail, burning... */
	system("flashnor /home/root/nor.bin-gta02v5-* && "
	       "touch /tmp/nor-done;");

        countdown(15, TRUE);

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
