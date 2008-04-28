#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "dma.h"
#include "hw.h"
#include "glamo.h"

#define DMA_CMDQ_SIZE	(1 * 1024)

static void dma_cmdq_init(struct glamo_dma_manager *dma)
{
	int cmdq_len;

	cmdq_len = (DMA_CMDQ_SIZE / 1024) - 1;

	dma->cmdq = (GLAMO_REG_TYPE *) (glamo_fb + GLAMO_VRAM_CMDQ);
	dma->size = DMA_CMDQ_SIZE;
	dma->r = 0;
	dma->w = 0;
	
	dma->cmdq[dma->size / 2] = 0x0;
	dma->cmdq[dma->size / 2 + 1] = 0x0;

	glamo_hw_engine_enable(GLAMO_ENGINE_CMDQ);
	glamo_hw_engine_reset(GLAMO_ENGINE_CMDQ);

        GLAMO_OUT_REG(GLAMO_REG_CMDQ_BASE_ADDRL,
		      GLAMO_VRAM_CMDQ & 0xffff);
        GLAMO_OUT_REG(GLAMO_REG_CMDQ_BASE_ADDRH,
		      (GLAMO_VRAM_CMDQ >> 16) & 0x7f);
        GLAMO_OUT_REG(GLAMO_REG_CMDQ_LEN, cmdq_len);

        GLAMO_OUT_REG(GLAMO_REG_CMDQ_WRITE_ADDRH, 0);
        GLAMO_OUT_REG(GLAMO_REG_CMDQ_WRITE_ADDRL, 0);
        GLAMO_OUT_REG(GLAMO_REG_CMDQ_READ_ADDRH, 0);
        GLAMO_OUT_REG(GLAMO_REG_CMDQ_READ_ADDRL, 0);
        GLAMO_OUT_REG(GLAMO_REG_CMDQ_CONTROL,
                      1 << 12 | /* turbo flip */
		      0 << 11 | /* no triple buffer */
                      5 << 8 | /* no interrupt */
                      8 << 4 | /* HQ threshold */
		      0 << 2 | /* no auto-correction */
		      0 << 1); /* SQ mode */
}

static void dma_cmdq_flush(struct glamo_dma_manager *dma)
{
	GLAMO_REG_TYPE *p = (GLAMO_REG_TYPE *) dma->lbuf;
	int ring_size = dma->size / 2;
	int ring_count = dma->llen / 2;

	if (ring_count == 0)
		return;

	/* write pointer can be ring_size, but not zero */
	while (ring_count--)
	{
		if (dma->w >= ring_size)
			dma->w = 0;

		dma->cmdq[dma->w++] = *p++;

		while (dma->r == dma->w)
		{
			dma->r  = GLAMO_IN_REG(GLAMO_REG_CMDQ_READ_ADDRL);
			dma->r |= (GLAMO_IN_REG(GLAMO_REG_CMDQ_READ_ADDRH) & 0x7) << 16;
		}
	}

	GLAMO_OUT_REG(GLAMO_REG_CMDQ_WRITE_ADDRH, (dma->w >> 15) & 0x7);
	GLAMO_OUT_REG(GLAMO_REG_CMDQ_WRITE_ADDRL, (dma->w <<  1) & 0xffff);

	dma->llen = 0;
}

static void dma_mmio_flush(struct glamo_dma_manager *dma)
{
	GLAMO_REG_TYPE *p = (GLAMO_REG_TYPE *) dma->lbuf;

	while ((unsigned char *) p < dma->lbuf + dma->llen)
	{
		GLAMO_REG_TYPE reg, val;

		reg = *p++;
		val = *p++;

		if (reg & (1 << 15))
		{
			int i, n = val;

			reg &= ~(1 << 15);

			for (i = 0; i < n; i++, reg += 2)
			{
				val = *p++;
				//printf("burst 0x%x to 0x%x\n", val, reg);
				GLAMO_OUT_REG(reg, val);
			}

			if (n & 1)
				p++;
		}
		else
			GLAMO_OUT_REG(reg, val);
	}

	dma->llen = 0;
}

void glamo_dma_wait(struct glamo_dma_manager *dma, enum glamo_dma_wait_type t)
{
	int mask, val;
	time_t begin, now;

	switch (dma->mode)
	{
	case GLAMO_DMA_MODE_CMDQ:
		switch (t)
		{
		case GLAMO_DMA_WAIT_CMDQ:
			mask = 0x3;
			val  = mask;
			break;
		case GLAMO_DMA_WAIT_ISP:
			mask = 0x3 | (1 << 8);
			val  = 0x3;
		case GLAMO_DMA_WAIT_ALL:
			mask = 1 << 2;
			val  = mask;
			break;
		}

		begin = time(NULL);
		while (1)
		{
			int status;

			status = GLAMO_IN_REG(GLAMO_REG_CMDQ_STATUS);
			if ((status & mask) == val)
				break;

			now = time(NULL);
			if (begin + 5 < now) {
				printf("wait timeout\n");

				break;
			}
		}
		break;
	case GLAMO_DMA_MODE_MMIO:
	default:
		break;
	}
}

void glamo_dma_flush(struct glamo_dma_manager *dma)
{
	if (dma->llen == 0)
		return;

	switch (dma->mode)
	{
	case GLAMO_DMA_MODE_CMDQ:
		dma_cmdq_flush(dma);
		break;
	case GLAMO_DMA_MODE_MMIO:
	default:
		dma_mmio_flush(dma);
		break;
	}
}

struct glamo_dma_manager *glamo_dma_new(enum glamo_dma_mode mode)
{
	struct glamo_dma_manager *dma;

	dma = malloc(sizeof(*dma));
	if (!dma)
		return NULL;

	dma->mode = mode;
	dma->lsize = DMA_CMDQ_SIZE;
	dma->lbuf = malloc(dma->lsize);
	dma->llen = 0;

	switch (mode)
	{
	case GLAMO_DMA_MODE_CMDQ:
		dma_cmdq_init(dma);
		break;
	case GLAMO_DMA_MODE_MMIO:
	default:
		break;
	}

	return dma;
}

void glamo_dma_destroy(struct glamo_dma_manager *dma)
{
	switch (dma->mode)
	{
	case GLAMO_DMA_MODE_CMDQ:
		glamo_hw_engine_reset(GLAMO_ENGINE_CMDQ);
		glamo_hw_engine_disable(GLAMO_ENGINE_CMDQ);
		break;
	case GLAMO_DMA_MODE_MMIO:
	default:
		break;
	}

	free(dma->lbuf);
	free(dma);
}

void glamo_dma_dump(struct glamo_dma_manager *dma)
{
	int i;

	switch (dma->mode)
	{
	case GLAMO_DMA_MODE_CMDQ:
		glamo_hw_dump(GLAMO_REG_CMDQ_BASE_ADDRL, 10);
		printf("w 0x%x, r 0x%x\n", dma->w << 1, dma->r << 1);
		for (i = 0; i < 16; i++)
			printf("0x%04x%c", dma->cmdq[i], ((i % 16) == 15) ? '\n' : ' ');
		break;
	case GLAMO_DMA_MODE_MMIO:
	default:
		break;
	}
}
