#include <stdio.h>
#include <unistd.h>
#include "glamo.h"
#include "hw.h"

#define STATUS_ENABLED	0x1
static int engine_status[GLAMO_ENGINE_2D + 1];

void glamo_hw_engine_reset(enum glamo_engine engine)
{
	int reg, mask;

	if (!(engine_status[engine] & STATUS_ENABLED))
		return;

	switch (engine) {
	case GLAMO_ENGINE_MPEG:
		reg = GLAMO_REG_CLOCK_MPEG;
		mask = GLAMO_CLOCK_MPEG_DEC_RESET;
		break;
	case GLAMO_ENGINE_ISP:
		reg = GLAMO_REG_CLOCK_ISP;
		mask = GLAMO_CLOCK_ISP2_RESET;
		break;
	case GLAMO_ENGINE_CMDQ:
		reg = GLAMO_REG_CLOCK_2D;
		mask = GLAMO_CLOCK_2D_CMDQ_RESET;
		break;
	case GLAMO_ENGINE_2D:
		reg = GLAMO_REG_CLOCK_2D;
		mask = GLAMO_CLOCK_2D_RESET;
		break;
	}

	glamo_set_bit_mask(reg, mask, 0xffff);
	usleep(1000);
	glamo_set_bit_mask(reg, mask, 0);
	usleep(1000);
}

void glamo_hw_engine_enable(enum glamo_engine engine)
{
	if (engine_status[engine] & STATUS_ENABLED)
		return;

	glamo_set_bit_mask(GLAMO_REG_CLOCK_GEN5_1, GLAMO_CLOCK_GEN51_EN_DIV_MCLK, 0xffff);

	switch (engine) {
	case GLAMO_ENGINE_MPEG:
		glamo_set_bit_mask(GLAMO_REG_CLOCK_MPEG,
				   GLAMO_CLOCK_MPEG_EN_X6CLK |
				   GLAMO_CLOCK_MPEG_DG_X6CLK |
				   GLAMO_CLOCK_MPEG_EN_X4CLK |
				   GLAMO_CLOCK_MPEG_DG_X4CLK |
				   GLAMO_CLOCK_MPEG_EN_X2CLK |
				   GLAMO_CLOCK_MPEG_DG_X2CLK |
				   GLAMO_CLOCK_MPEG_EN_X0CLK |
				   GLAMO_CLOCK_MPEG_DG_X0CLK,
				   0xffff & ~GLAMO_CLOCK_MPEG_DG_X0CLK);
		glamo_set_bit_mask(GLAMO_REG_CLOCK_MPROC,
				   GLAMO_CLOCK_MPROC_EN_M4CLK,
				   0xffff);
		glamo_set_bit_mask(GLAMO_REG_CLOCK_GEN5_1,
				   GLAMO_CLOCK_GEN51_EN_DIV_JCLK,
				   0xffff);
		glamo_set_bit_mask(GLAMO_REG_HOSTBUS(2),
				   GLAMO_HOSTBUS2_MMIO_EN_MPEG |
				   GLAMO_HOSTBUS2_MMIO_EN_MICROP1,
				   0xffff);
		glamo_set_bit_mask(GLAMO_REG_CLOCK_MPROC,
				   GLAMO_CLOCK_MPROC_EN_KCLK,
				   0xffff);
		break;
	case GLAMO_ENGINE_ISP:
		glamo_set_bit_mask(GLAMO_REG_CLOCK_ISP,
				   GLAMO_CLOCK_ISP_EN_M2CLK |
				   GLAMO_CLOCK_ISP_EN_I1CLK,
				   0xffff);
		glamo_set_bit_mask(GLAMO_REG_CLOCK_GEN5_2,
				   GLAMO_CLOCK_GEN52_EN_DIV_ICLK,
				   0xffff);
		glamo_set_bit_mask(GLAMO_REG_CLOCK_GEN5_1,
				   GLAMO_CLOCK_GEN51_EN_DIV_JCLK,
				   0xffff);
		glamo_set_bit_mask(GLAMO_REG_HOSTBUS(2),
				   GLAMO_HOSTBUS2_MMIO_EN_ISP,
				   0xffff);
		break;
	case GLAMO_ENGINE_CMDQ:
		glamo_set_bit_mask(GLAMO_REG_CLOCK_2D,
				   GLAMO_CLOCK_2D_EN_M6CLK,
				   0xffff);
		glamo_set_bit_mask(GLAMO_REG_HOSTBUS(2),
				   GLAMO_HOSTBUS2_MMIO_EN_CMDQ,
				   0xffff);
		break;
	case GLAMO_ENGINE_2D:
		glamo_set_bit_mask(GLAMO_REG_CLOCK_2D,
				   GLAMO_CLOCK_2D_EN_M7CLK |
				   GLAMO_CLOCK_2D_EN_GCLK |
				   GLAMO_CLOCK_2D_DG_M7CLK |
				   GLAMO_CLOCK_2D_DG_GCLK,
				   0xffff);
		glamo_set_bit_mask(GLAMO_REG_HOSTBUS(2),
				   GLAMO_HOSTBUS2_MMIO_EN_2D,
				   0xffff);
		glamo_set_bit_mask(GLAMO_REG_CLOCK_GEN5_1,
				   GLAMO_CLOCK_GEN51_EN_DIV_GCLK,
				   0xffff);
		break;
	}

	engine_status[engine] |= STATUS_ENABLED;
}

void glamo_hw_engine_disable(enum glamo_engine engine)
{
	if (!(engine_status[engine] & STATUS_ENABLED))
		return;

	switch (engine) {
	case GLAMO_ENGINE_MPEG:
		glamo_set_bit_mask(GLAMO_REG_CLOCK_MPEG,
				   GLAMO_CLOCK_MPEG_EN_X6CLK |
				   GLAMO_CLOCK_MPEG_DG_X6CLK |
				   GLAMO_CLOCK_MPEG_EN_X4CLK |
				   GLAMO_CLOCK_MPEG_DG_X4CLK |
				   GLAMO_CLOCK_MPEG_EN_X2CLK |
				   GLAMO_CLOCK_MPEG_DG_X2CLK |
				   GLAMO_CLOCK_MPEG_EN_X0CLK |
				   GLAMO_CLOCK_MPEG_DG_X0CLK,
				   0);
		glamo_set_bit_mask(GLAMO_REG_CLOCK_MPROC,
				   GLAMO_CLOCK_MPROC_EN_M4CLK,
				   0);
		glamo_set_bit_mask(GLAMO_REG_CLOCK_GEN5_1,
				   GLAMO_CLOCK_GEN51_EN_DIV_JCLK,
				   0);
		glamo_set_bit_mask(GLAMO_REG_HOSTBUS(2),
				   GLAMO_HOSTBUS2_MMIO_EN_MPEG |
				   GLAMO_HOSTBUS2_MMIO_EN_MICROP1,
				   0);
		glamo_set_bit_mask(GLAMO_REG_CLOCK_MPROC,
				   GLAMO_CLOCK_MPROC_EN_KCLK,
				   0);
		break;
	case GLAMO_ENGINE_ISP:
		glamo_set_bit_mask(GLAMO_REG_CLOCK_ISP,
				   GLAMO_CLOCK_ISP_EN_M2CLK |
				   GLAMO_CLOCK_ISP_EN_I1CLK,
				   0);
		glamo_set_bit_mask(GLAMO_REG_CLOCK_GEN5_2,
				   GLAMO_CLOCK_GEN52_EN_DIV_ICLK,
				   0);
		glamo_set_bit_mask(GLAMO_REG_CLOCK_GEN5_1,
				   GLAMO_CLOCK_GEN51_EN_DIV_JCLK,
				   0);
		glamo_set_bit_mask(GLAMO_REG_HOSTBUS(2),
				   GLAMO_HOSTBUS2_MMIO_EN_ISP,
				   0);
		break;
	case GLAMO_ENGINE_CMDQ:
		glamo_set_bit_mask(GLAMO_REG_CLOCK_2D,
				   GLAMO_CLOCK_2D_EN_M6CLK,
				   0);
		glamo_set_bit_mask(GLAMO_REG_HOSTBUS(2),
				   GLAMO_HOSTBUS2_MMIO_EN_CMDQ,
				   0);
		break;
	case GLAMO_ENGINE_2D:
		glamo_set_bit_mask(GLAMO_REG_CLOCK_2D,
				   GLAMO_CLOCK_2D_EN_M7CLK |
				   GLAMO_CLOCK_2D_EN_GCLK |
				   GLAMO_CLOCK_2D_DG_M7CLK |
				   GLAMO_CLOCK_2D_DG_GCLK,
				   0);
		glamo_set_bit_mask(GLAMO_REG_HOSTBUS(2),
				   GLAMO_HOSTBUS2_MMIO_EN_2D,
				   0);
		glamo_set_bit_mask(GLAMO_REG_CLOCK_GEN5_1,
				   GLAMO_CLOCK_GEN51_EN_DIV_GCLK,
				   0);
		break;
	}

	/* simply commented out;  we don't want a clock manager to do things right */
	//glamo_set_bit_mask(GLAMO_REG_CLOCK_GEN5_1, GLAMO_CLOCK_GEN51_EN_DIV_MCLK, 0);

	engine_status[engine] &= ~STATUS_ENABLED;
}

void glamo_hw_dump(int reg, int len)
{
	int i, val;

	printf("reg       val\n");

	for (i = 0; i < len; i++, reg += 2)
	{
		val = GLAMO_IN_REG(reg);

		printf("0x%06x: 0x%04x\n", reg, val);
	}
}
