#include <common.h>
#include <command.h>
#include <s3c2410.h>
#include <s3c24x0.h>

int do_earphone (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	S3C24X0_GPIO * const gpio = S3C24X0_GetBase_GPIO();

	if(argc > 1)
	goto out_help;

	printf("[EARPHONE] start test\n");
	printf("[EARPHONE] Waiting For Ear Phone Plug\n");
	gpio->GPJDAT |= (1 << 6);
	udelay(5000000);
	while(neo1973_earphone_pressed());
	gpio->GPJDAT &= ~(1 << 6);
	printf("[EARPHONE] Ear Phone Plug in!!\n");
	printf("[EARPHONE] end test\n");
	return 0;

out_help:
	printf ("Usage:\n%s\n", cmdtp->usage);
	return 1;
}

int neo1973_earphone_pressed(void)
{
        S3C24X0_GPIO * const gpio = S3C24X0_GetBase_GPIO();

        if (gpio->GPFDAT & (1 << 4))
                return 0;
        return 1;
}

/***************************************************/

U_BOOT_CMD(
        earphone,    CFG_MAXARGS,    1,      do_earphone,
        "earphone- test ear phone pluged\n",
);
