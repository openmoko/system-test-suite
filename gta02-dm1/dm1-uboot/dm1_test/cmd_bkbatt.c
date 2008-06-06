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
#include <pcf50633.h>

int BackupBattery (int flag)
{
	pcf50633_reg_set_bit_mask(PCF50633_REG_BBCCTL,
                                           0x01, flag);
	return 0;
}

int get_state(void)
{
        u_int8_t val = pcf50633_reg_read(PCF50633_REG_BBCCTL);

	if((val&0x1) == 0x1) 
		return 1;
	else
		return 0;
}


int do_backupbattery (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int i;

        if(argc > 1)
        goto out_help;

	printf("===================================\n");
	printf("[BACKUPBATTERY] start test\n");
	BackupBattery(1);
	udelay(50000);
	printf("[BACKUPBATTERY] check\n");
	if(get_state())
		printf("[BACKUPBATTERY] Pass\n");
	else 
		printf("[BACKUPBATTERY] Fail");
	printf("[BACKUPBATTERY] end test\n");
	printf("EOF\n");
	printf("===================================\n");

        return 0;

out_help:
        printf ("Usage:\n%s\n", cmdtp->usage);
        return 1;	
}


/***************************************************/

U_BOOT_CMD(
	backupbatt,    CFG_MAXARGS,      1,      do_backupbattery,
        "backupbatt     - charger in backupbattery\n",

);


