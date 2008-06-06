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

#include "pcf50633.h"

int do_usb_5v (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	if(argc < 2)
		goto out_help;

	if (!strcmp(argv[1], "on")) {
		pcf50633_reg_write(PCF50633_REG_MBCC7, 0x3);
		pcf50633_reg_write(PCF50633_REG_MBCC8, 0x0);
		pcf50633_reg_write(PCF50633_REG_GPOCFG, 0x7);
	} else if (!strcmp(argv[1], "off")) {
		pcf50633_reg_write(PCF50633_REG_GPOCFG, 0x0);
		pcf50633_reg_write(PCF50633_REG_MBCC7, 0x0);
		pcf50633_reg_write(PCF50633_REG_MBCC8, 0x0);
	}

	return 0;

out_help:
	printf ("Usage:\n%s\n", cmdtp->usage);
	return 1;
}

U_BOOT_CMD(
	usb5v,    2,      1,      do_usb_5v,
	"usb5v   - set USB host 5v\n",
	" [on/off] \n on    - usb 5v\n"
	"off    - usb 0v \n"
);
