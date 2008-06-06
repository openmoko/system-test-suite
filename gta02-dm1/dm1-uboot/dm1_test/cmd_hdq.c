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

void hdq_send_zero(void)
{
	S3C24X0_GPIO * const gpio = S3C24X0_GetBase_GPIO();
	int i;
	/* output -- low */
	gpio->GPDCON &= ~(0x30000000);
	gpio->GPDCON |= (1 << 28);
	gpio->GPDDAT &= ~(1 << 14); /* send data 0 */

	for(i = 0; i < 400; i++)
		gpio->GPDCON |= (1 << 28);
	/* input -- high */
	gpio->GPDCON &= ~(0x30000000);
	gpio->GPDUP |= (1 << 14);

	for(i = 0; i < 360; i++)
		gpio->GPDCON |= (0 << 28);
}

void hdq_send_one(void)
{
	S3C24X0_GPIO * const gpio = S3C24X0_GetBase_GPIO();
	int i;
	/* output -- low */
	gpio->GPDCON &= ~(0x30000000);
	gpio->GPDCON |= (1 << 28);
	gpio->GPDDAT &= ~(1 << 14); /* send data 0 */

	for(i = 0; i < 100; i++)
		gpio->GPDCON |= (1 << 28);
	/* input -- high */
	gpio->GPDCON &= ~(0x30000000);
	gpio->GPDUP |= (1 << 14);

	for(i = 0; i < 660; i++)
		gpio->GPDCON |= (0 << 28);
}

void hdq_read_data(void)
{
	S3C24X0_GPIO * const gpio = S3C24X0_GetBase_GPIO();
	int i;

	for (i = 0; i < 8; i++) {
		if((gpio->GPDDAT & (1 << 14)))
			printf("1");
                else
			printf("0");
	}
	printf("\n");
}

void neo1973_hdq(void)
{
	S3C24X0_GPIO * const gpio = S3C24X0_GetBase_GPIO();
	int i;

	printf("HDQ test ##\n");
	/* GPD14 */


	/* Break Time before sending data */
	gpio->GPDCON &= ~(0x30000000);
	gpio->GPDCON |= (1 << 28);
	gpio->GPDDAT &= ~(1 << 14); /* send data 0 */

	for(i = 0; i < 800; i++)
		gpio->GPDCON |= (1 << 28);

	gpio->GPDCON &= ~(0x30000000);
	gpio->GPDUP |= (1 << 14);

	for(i =0; i < 160; i++)
		gpio->GPDCON |= (0 << 28);

	/* Send Data : 00110100 */
	hdq_send_zero();
	hdq_send_zero();
	hdq_send_one();
	hdq_send_one();
	hdq_send_zero();
	hdq_send_one();
	hdq_send_zero();
	hdq_send_zero();

	/* Read Data */
	hdq_read_data();
}

int do_hdq (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	if(argc > 1)
		goto out_help;

	printf("===================================\n");
	printf("[HDQ] start test\n");
	neo1973_hdq();
	printf("[HDQ] end test\n");
	printf("EOF\n");
	printf("===================================\n");

        return 0;

out_help:
        printf ("Usage:\n%s\n", cmdtp->usage);
        return 1;	
}

/***************************************************/

U_BOOT_CMD(
	hdq,    CFG_MAXARGS,      1,      do_hdq,
        "hdq     - hdq on/off\n",

);

