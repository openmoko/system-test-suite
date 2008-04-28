#include <stdlib.h>
#include <stdio.h>
#include "libglamo/glamo.h"
#include "libglamo/hw.h"
#include "libglamo/dma.h"
#include "dm_init.h"

#define VRAM_LOOP 1
#define CMDQ_LOOP 100

#define REPORT_OK(item) \
	printf("[GRAPHICS] " item RET_OK)
#define REPORT_FAIL(item) \
	printf("[GRAPHICS] " item RET_FAIL)

static int vram_test(void)
{
	int i, j;

	for (i = 0; i < VRAM_LOOP; i++)
	{
		volatile unsigned char *fb;

		fb = (unsigned char *) (glamo_fb + GLAMO_VRAM_FB);
		for (j = GLAMO_VRAM_CMDQ; j < GLAMO_VRAM_ISP; j++)
		{
			int v, r;

			v = (j + i) & 0xff;

			fb[j] = v;
			r = fb[j];

			if (v != r)
			{
				REPORT_FAIL("VRAM");

				return 0;
			}
		}
	}

	REPORT_OK("VRAM");

	return 1;
}

static int cmdq_test(struct glamo_dma_manager *dma)
{
	int i, val;
	GLAMO_DMA_VARS;

	for (i = 0; i < CMDQ_LOOP; i++)
	{
		GLAMO_DMA_BEGIN(dma, 4);
		GLAMO_DMA_OUT(dma, 0, 0);
		GLAMO_DMA_OUT(dma, 0, 0);
		GLAMO_DMA_END(dma);

		glamo_dma_flush(dma);

		val = GLAMO_IN_REG(GLAMO_REG_CMDQ_WRITE_ADDRL);
		if (val != dma->w << 1)
		{
			REPORT_FAIL("CMDQ");

			return 0;
		}

		glamo_dma_wait(dma, GLAMO_DMA_WAIT_ALL);
	}

	REPORT_OK("CMDQ");

	return 1;
}

void graphics_test(void)
{
	struct glamo_dma_manager *dma;

	glamo_os_init();

	if (!vram_test())
		goto out1;

	dma = glamo_dma_new(GLAMO_DMA_MODE_CMDQ);
	if (!dma)
	{
		REPORT_FAIL("CMDQ");

		goto out1;
	}

	if (!cmdq_test(dma))
		goto out2;

out2:
	glamo_dma_destroy(dma);
out1:
	glamo_os_finish();
}
