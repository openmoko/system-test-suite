#include <common.h>
#include <command.h>
#include <s3c2410.h>
#include <s3c24x0.h>

int neo1973_auxkey_pressed(void)
{
	S3C24X0_GPIO * const gpio = S3C24X0_GetBase_GPIO();

	if (gpio->GPFDAT & (1 << 6))
		return 0;
	return 1;
}

int do_key911 (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	if(argc > 1)
	goto out_help;

	printf("[KEY911] start test\n");
	printf("[KEY911] Waiting For Key Press\n");
	while(neo1973_auxkey_pressed());
	printf("[KEY911] Key Press Down!!\n");
	printf("[KEY911] end test\n");
	return 0;

out_help:
	printf ("Usage:\n%s\n", cmdtp->usage);
	return 1;
}

/***************************************************/

U_BOOT_CMD(
        key911,    CFG_MAXARGS,    1,      do_key911,
        "key911     - test 911 key pressed\n",
);

