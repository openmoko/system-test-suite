/*
 * (C) Copyright 2005
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <s3c2440.h>

#include "../board/neo1973/common/neo1973.h"

#define	NEO1973_CHGCMD_GET	NEO1973_CHGCMD_NONE

int do_charge (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
        if(argc < 1)
        goto out_help;

	printf("===================================\n");
	printf("[CHARGE] start test\n");
	charge(NEO1973_CHGCMD_OFF);
	charge(NEO1973_CHGCMD_GET);
	charge(NEO1973_CHGCMD_FAST);
	charge(NEO1973_CHGCMD_GET);
	printf("[CHARGE] charging....\n");
	udelay(5000000);
	udelay(5000000);
	charge(NEO1973_CHGCMD_OFF);
	charge(NEO1973_CHGCMD_GET);
	printf("[CHARGE] end test\n");
	printf("EOF\n");
	printf("===================================\n");

	return 0;

out_help:
        printf ("Usage:\n%s\n", cmdtp->usage);
        return 1;    
}

int charge(int cmd)
{
	switch(cmd) {
	case NEO1973_CHGCMD_OFF:
		neo1973_set_charge_mode(NEO1973_CHGCMD_OFF);
		break;
	case NEO1973_CHGCMD_FAST:
		neo1973_set_charge_mode(NEO1973_CHGCMD_FAST);
		break;
	case NEO1973_CHGCMD_GET:
		printf("[CHARGE] charge status : %s\n", neo1973_get_charge_status());
		break;
	}
}

/***************************************************/

U_BOOT_CMD(
        charge,    CFG_MAXARGS,      1,      do_charge,
        "charge     - set/get charge mode\n",
);

