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

#define NORFLASH_BASE	0x18000000
#define DISP_LINE_LEN	16
#define DEVICE_CODE	"18000000: 88130020"

static	ulong	base_address = 0;

int do_nor_check (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
        if(argc < 2)
        goto out_help;

	if (!strcmp(argv[1], "check")) {
		mww(NORFLASH_BASE, 0x90);
		mdw(NORFLASH_BASE, 0x1);
	}

	return 0;

out_help:
        printf ("Usage:\n%s\n", cmdtp->usage);
        return 1;    
}

int mww (ulong addr, ulong writeval)
{
        ulong   count;
	int	size;

        addr += base_address;
	count = 1;
	size = 4;

        while (count-- > 0) {
                *((ulong  *)addr) = (ulong )writeval;
                addr += size;
        }
        return 0;
}

int mdw (ulong addr, ulong length)
{
        int     size;

	size = 4;
	addr += base_address;
	
	check_buffer(addr, (void*)addr, size, length, DISP_LINE_LEN/size);
        addr += size*length;

        return 0;
}

#define MAX_LINE_LENGTH_BYTES (64)
#define DEFAULT_LINE_LENGTH_BYTES (16)
int check_buffer (ulong addr, void* data, uint width, uint count, uint linelen)
{
	char buf[20], tmp[20];
	uint8_t linebuf[MAX_LINE_LENGTH_BYTES];
	uint32_t *uip = (void*)linebuf;
	uint16_t *usp = (void*)linebuf;
	uint8_t *ucp = (void*)linebuf;
	int i;

	while (count) {
		sprintf(buf, "%08lx:", addr);

		/* check for overflow condition */
		if (count < linelen)
			linelen = count;

		/* Copy from memory into linebuf and print hex values */
		for (i = 0; i < linelen; i++) {
			if (width == 4) {
				uip[i] = *(volatile uint32_t *)data;
				sprintf(tmp, " %08x", uip[i]);
			} else if (width == 2) {
				usp[i] = *(volatile uint16_t *)data;
				sprintf(tmp, " %04x", usp[i]);
			} else {
				ucp[i] = *(volatile uint8_t *)data;
				sprintf(tmp, " %02x", ucp[i]);
			}
			data += width;
		}
		strcat(buf, tmp);

		printf("===================================\n");
		printf("[NORFLASH] start test\n");
		printf("[NORFLASH] ");
		if(!strcmp(buf, DEVICE_CODE))
			printf("NOR Flash Verification Pass\n");
		else
			printf("NOR Flash Verification Fail\n");
		printf("[NORFLASH] end test\n");
		printf("EOF\n");
		printf("===================================\n");

		/* update references */
		addr += linelen * width;
		count -= linelen;

		if (ctrlc())
			return -1;
	}

	return 0;
}


/***************************************************/

U_BOOT_CMD(
        nor,    2,      1,      do_nor_check,
        "nor     - check NOR Flash\n",
        "check   \n"
);

