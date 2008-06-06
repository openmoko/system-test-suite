/* u-boot driver for MEMS motion sensor */
#include <common.h>
#include <spi.h>
//#include <s3c2410.h>
#include "lis302dl.h"
#include <command.h>
#include <s3c2440.h>

enum lis_register {
	LIS_REG_WHOAMI			= 0x0F,

	LIS_REG_CTRL_1			= 0x20,
	LIS_REG_CTRL_2			= 0x21,
	LIS_REG_CTRL_3			= 0x22,

	LIS_REG_FILTER_RESET		= 0x23,

	LIS_REG_STATUS			= 0x27,

	LIS_REG_OUTX			= 0x29,
	LIS_REG_OUTY			= 0x2B,
	LIS_REG_OUTZ			= 0x2D,

	LIS_REG_FF_WU_CFG_1		= 0x30,
	LIS_REG_FF_WU_SRC_1		= 0x31,
	LIS_REG_FF_WU_THS_1		= 0x32,
	LIS_REG_FF_WU_DURATION_1	= 0x33,

	LIS_REG_FF_WU_CFG_2		= 0x34,
	LIS_REG_FF_WU_SRC_2		= 0x35,
	LIS_REG_FF_WU_THS_2		= 0x36,
	LIS_REG_FF_WU_DURATION_2	= 0x37,

	LIS_REG_CLICK_CFG		= 0x38,
	LIS_REG_CLICK_SRC		= 0x39,

	LIS_REG_CLICK_THSY_X		= 0x3B,
	LIS_REG_CLICK_THSZ		= 0x3C,
	LIS_REG_CLICK_TIMELIMIT		= 0x3D,
	LIS_REG_CLICK_LATENCY		= 0x3E,
	LIS_REG_CLICK_WINDOW		= 0x3F,
};


#define GTA02_SCLK	(1 << 7)	/* GPG7 */
#define GTA02_MOSI	(1 << 6)	/* GPG6 */
#define GTA02_MISO	(1 << 5)	/* GPG5 */

#define GTA02_G1_CS	(1 << 12)	/* GPD12 */
#define GTA02_G2_CS	(1 << 13)	/* GPD13 */

#define SPI_G1_CS(bit)	if (bit) gpio->GPDDAT |= GTA02_G1_CS; \
			else	gpio->GPDDAT &= ~GTA02_G1_CS

#define SPI_G2_CS(bit)	if (bit) gpio->GPDDAT |= GTA02_G2_CS; \
			else	gpio->GPDDAT &= ~GTA02_G2_CS

#define SPI_SDA(bit)	if (bit) gpio->GPGDAT |= GTA02_MOSI; \
			else	gpio->GPGDAT &= ~GTA02_MOSI

#define SPI_SCL(bit)	if (bit) gpio->GPGDAT |= GTA02_SCLK; \
			else	gpio->GPGDAT &= ~GTA02_SCLK

/* 150uS minimum clock cycle, we have two of this plus our other
 * instructions */
#define SPI_DELAY	udelay(100)	/* 200uS */

/* ABS */
#define ABS(a)	(((a) < 0) ? -(a) : (a))

/* Fixed me */
#define SPI_READ	((gpio->GPGDAT & GTA02_MISO) != 0)

struct lis_info {
	u_int16_t tx_buf[4];
	u_int16_t rx_buf[4];
	struct spi_device *spi_dev;
};

static int lis_spi_snd(int wordnum, int bitlen, u_int16_t *dout)
{
	S3C24X0_GPIO * const gpio = S3C24X0_GetBase_GPIO();
	u_int16_t tmpout;
	int i, j;

	for(i = 0; i < wordnum; i++) {
		tmpout = dout[i];

		for (j = 0; j < bitlen; j++ ) {
			SPI_SCL(0);
			if (tmpout & (1 << bitlen - 1)) {
				SPI_SDA(1);
			} else {
				SPI_SDA(0);
			}
			SPI_DELAY;
			SPI_SCL(1);
			SPI_DELAY;
			tmpout <<= 1;
		}
	}

	return 0;
}

static int lis_spi_rev(int wordnum, int bitlen, u_int16_t *din)
{
	S3C24X0_GPIO * const gpio = S3C24X0_GetBase_GPIO();
	u_int16_t tmpin = 0x0;
	int i,j, tmpnum;

	tmpnum = wordnum;
	for (i = 0; i < wordnum; i++) {
		for(j = 0; j < bitlen; j++) {
			SPI_SCL(0);
			if (SPI_READ) {
				tmpin |= 0x1;
			} else {
				tmpin &= ~(0x01);
			}
			SPI_DELAY;
			SPI_SCL(1);
			SPI_DELAY;
			if (j != (bitlen-1)) {
				tmpin <<= 1;
				}
			}
		din[--tmpnum] = tmpin;
		tmpin = 0x0;
	}

	return 0;
}


int sensor_spi_init(void)
{
        S3C24X0_GPIO * const gpio = S3C24X0_GetBase_GPIO();

        /* initialize SPI for GPIO bitbang */
        gpio->GPGCON &= 0xfffc03ff;
        gpio->GPGCON |= 0x00005000;

	gpio->GPFCON &= 0xfffffffc;

	gpio->GPFUP |= 0x1;

	/* initialize chip-select (CS) high level */
	SPI_G1_CS(1);
	SPI_G2_CS(1);

        udelay(100);

        return 0;
}


int lis_reg_read(int lisno, u_int16_t reg)
{
        S3C24X0_GPIO * const gpio = S3C24X0_GetBase_GPIO();
	u_int16_t snd_data[1], rev_data[1];

	snd_data[0] = (1 << 7)|(0 << 6)|(reg & 0x3F);
	rev_data[0] = 0x00;

	if (lisno == 1)
		SPI_G1_CS(0);
	else if (lisno == 2)
		SPI_G2_CS(0);
	else
		printf("lis302dl number is wrong\n");

	lis_spi_snd( 1, 8, snd_data);
	lis_spi_rev( 1, 8, rev_data);

	if (lisno == 1)
		SPI_G1_CS(1);
	else if (lisno == 2)
		SPI_G2_CS(1);

	return rev_data[0];
}

int lis_reg_write(int lisno, u_int16_t reg, u_int16_t data)
{
        S3C24X0_GPIO * const gpio = S3C24X0_GetBase_GPIO();
	u_int16_t snd_data[1];

	snd_data[0] = (0 << 15)|(0 << 14)|((reg & 0x3F) << 8)|(data & 0xFF);
	if (lisno == 1)
		SPI_G1_CS(0);
	else if (lisno == 2)
		SPI_G2_CS(0);
	else
		printf("lis302dl number is wrong\n");

	lis_spi_snd(1, 16, snd_data);

	if (lisno == 1)
		SPI_G1_CS(1);
	else if (lisno == 2)
		SPI_G2_CS(1);

	return 0;
}

int lis_self_test(int lisno) {
	int i;
	int8_t a, b, c, d, e, f, v;

	printf("value : %x\n", lis_reg_read(lisno, 0x20));

	lis_reg_write(lisno, 0x21, 0x00);
	lis_reg_write(lisno, 0x22, 0x00);
	lis_reg_write(lisno, 0x20, 0x47);

	udelay(60000);

	//printf("read status %x\n", lis_reg_read(lisno, 0x27));
	while (!((lis_reg_read(lisno, 0x27) & 0x8) >> 3));

	printf("who_am_i : %x\n", v=lis_reg_read(lisno, 0x0F));

	if((v&0xff) == 0xff) goto fail;

	//printf("first xyz value\n");
	a = (lis_reg_read(lisno, 0x29));
	b = (lis_reg_read(lisno, 0x2B));
	c = (lis_reg_read(lisno, 0x2D));
        //printf("1 OUT X : %x \n", a);
        //printf("1 OUT Y : %x \n", b);
        //printf("1 OUT Z : %x \n", c);

	printf("1 OUT X : %d \n", (a));
	printf("1 OUT Y : %d \n", (b));
	printf("1 OUT Z : %d \n", (c));

//	if (ABS(a) > 83 ||
//			ABS(b) > 83 ||
//			ABS(c) > 83)
//		goto fail;

	lis_reg_write(lisno, 0x20, 0x57);

	//printf("read status %x\n", lis_reg_read(lisno, 0x27));
	while (!((lis_reg_read(lisno, 0x27) & 0x8) >> 3));

	for (i = 0; i < 5; i++) {
	lis_reg_read(lisno, 0x29);
	lis_reg_read(lisno, 0x2B);
	lis_reg_read(lisno, 0x2D);
	while (!((lis_reg_read(lisno, 0x27) & 0x8) >> 3));
	//printf("OUT X : %x \n", lis_reg_read(lisno, 0x29));
	//printf("OUT Y : %x \n", lis_reg_read(lisno, 0x2B));
	//printf("OUT Z : %x \n", lis_reg_read(lisno, 0x2D));
	}

	//printf("final xyz value\n");
        d = (lis_reg_read(lisno, 0x29));
        e = (lis_reg_read(lisno, 0x2B));
        f = (lis_reg_read(lisno, 0x2D));

        //printf("2 OUT X : %x \n", a);
        //printf("2 OUT Y : %x \n", b);
        //printf("2 OUT Z : %x \n", c);

        printf("2 OUT X : %d \n", (d));
        printf("2 OUT Y : %d \n", (e));
        printf("2 OUT Z : %d \n", (f));


	printf("value : %x\n", lis_reg_read(lisno, 0x20));
//	if (((3 > ABS(d-a)) || (ABS(d-a) > 16)) ||
//	    ((3 > ABS(e-b)) || (ABS(e-b) > 16)) ||
//	    ((3 > ABS(f-c)) || (ABS(f-c) > 16)))
//	goto fail;


//	     ((3 <= ABS(e-b)) && (ABS(e-b) <= 16)) ||
//	     ((3 <= ABS(f-c)) && (ABS(f-c) <= 16)))
//	goto fail;

	/* Disable Sensor */
	lis_reg_write(lisno, 0x20, 0x00);

	printf("pass!! \n");
	return 0;
fail:
	/* Disable Sensor */
	lis_reg_write(lisno, 0x20, 0x00);
	printf("fail!! \n");
	return 0;
}

int lis_interrupt_test(int lisno, int INTno) {
        S3C24X0_GPIO * const gpio = S3C24X0_GetBase_GPIO();

//	lis_reg_write(1, 0x20, 0x00);

	if (lisno == 1) {
		gpio->GPFCON &= 0xfffffffc;
		gpio->GPFUP |= 0x1;
	} else if (lisno == 2) {
		gpio->GPGCON &= 0xfffcffff;
		gpio->GPFUP |=  0x00010000;
	}

	udelay(600);

	lis_reg_write(lisno, 0x20, 0x47);
	if(gta02_revision == 6)
		lis_reg_write(lisno, 0x22, 0x80);
	else if(gta02_revision == 5)
		lis_reg_write(lisno, 0x22, 0xc0);

	/* INT1 */
	if (INTno == 1) {
		lis_reg_write(lisno, 0x32, 0x14);
		lis_reg_write(lisno, 0x33, 0x00);
		lis_reg_write(lisno, 0x30, 0x95);

		if(gta02_revision == 6)
			lis_reg_write(lisno, 0x22, 0x81);
		else if(gta02_revision == 5)
			lis_reg_write(lisno, 0x22, 0xc1);
	}

	/* INT2 */
	if (INTno == 2) {
		lis_reg_write(lisno, 0x36, 0x14);
		lis_reg_write(lisno, 0x37, 0x00);
		lis_reg_write(lisno, 0x34, 0x95);
		
		if(gta02_revision == 6)
			lis_reg_write(lisno, 0x22, 0x90);
		else if(gta02_revision == 5)
			lis_reg_write(lisno, 0x22, 0xd0);
	}

	udelay(60000);
	/* check interrupt */
	if (lisno == 1) {
		if ((gpio->GPFDAT & 0x01) != 0) // high -> low :interrupt , low -> high : no interrupt
			printf("first check : no interrupt !!\n");
		else
			printf("first check : interrupt !!\n");
	}

	if (lisno == 2) {
		if ((gpio->GPGDAT & 0x100) != 0)
			printf("first check : no interrupt !!\n");
		else
			printf("first check : interrupt !!\n");
	}

	udelay(600);

	if(gta02_revision == 6)
		lis_reg_write(lisno, 0x22, 0x80);
	else if(gta02_revision == 5)
		lis_reg_write(lisno, 0x22, 0xc0);

	/* INT1 */
	if (INTno == 1) {
		lis_reg_write(lisno, 0x32, 0x54);
		if(gta02_revision == 6)
			lis_reg_write(lisno, 0x22, 0x81);
		else if(gta02_revision == 5)
			lis_reg_write(lisno, 0x22, 0xc1);
	}

	/* INT2 */
	if (INTno == 2) {
		lis_reg_write(lisno, 0x36, 0x54);
		if(gta02_revision == 6)
			lis_reg_write(lisno, 0x22, 0x90);
		else if(gta02_revision == 5)
			lis_reg_write(lisno, 0x22, 0xd0);
	}

	udelay(60000);

	/* check interrupt */
        if (lisno == 1) {
                if ((gpio->GPFDAT & 0x01) != 0)
                        printf("first check : no interrupt !!\n");
                else
                        printf("first check : interrupt !!\n");
        }

        if (lisno == 2) {
                if ((gpio->GPGDAT & 0x100) != 0)
                        printf("first check : no interrupt !!\n");
                else
                        printf("first check : interrupt !!\n");
        }

	lis_reg_write(lisno, 0x20, 0x00);
	return 0;
}

int lis_init(void) {
	printf("[SENSOR] start test\n");
	printf("Init LIS302DL\n");
	sensor_spi_init();
	lis_self_test(1);
	lis_interrupt_test(1,1);
	lis_interrupt_test(1,2);

	lis_self_test(2);
	lis_interrupt_test(2,1);
	lis_interrupt_test(2,2);
	printf("[SENSOR] end test\n");
	return 0;
}

int do_sensor (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	if(argc > 1)
	goto out_help;

	lis_init();

	return 0;

out_help:
	printf ("Usage:\n%s\n", cmdtp->usage);
	return 1;
}


/***************************************************/

U_BOOT_CMD(
	sensor,    CFG_MAXARGS,    1,      do_sensor,
	"sensor     - test sensor\n",
);
