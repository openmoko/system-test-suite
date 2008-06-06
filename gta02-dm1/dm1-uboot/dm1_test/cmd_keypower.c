#include <common.h>
#include <command.h>
#include <s3c2410.h>
#include <s3c24x0.h>
#include "../board/neo1973/common/neo1973.h"

int do_keypower (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
        if(argc > 1)
        goto out_help;

	printf("[KEYPOWER] start test\n");
        printf("[KEYPOWER] Waiting For Key Press\n");
        while(!neo1973_on_key_pressed());
        printf("[KEYPOWER] Key Press Down!!\n");
	printf("[KEYPOWER] end test\n");
        return 0;

out_help:
        printf ("Usage:\n%s\n", cmdtp->usage);
        return 1;
}

/***************************************************/

U_BOOT_CMD(
        keypower,    CFG_MAXARGS,    1,      do_keypower,
        "keypower - test power key pressed\n",
);

