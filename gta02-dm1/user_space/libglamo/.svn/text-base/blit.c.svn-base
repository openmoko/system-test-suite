#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "glamo.h"
#include "hw.h"
#include "dma.h"
#include "blit.h"

static void blit_set_onfly_regs(struct glamo_dma_manager *dma)
{
	struct {
		int src_block_x;
		int src_block_y;
		int src_block_w;
		int src_block_h;
		int jpeg_out_y;
		int jpeg_out_x;
		int fifo_full_cnt;
		int in_length;
		int fifo_data_cnt;
		int in_height;
	} onfly;
	GLAMO_DMA_VARS;

	onfly.src_block_y = 32;
	onfly.src_block_x = 32;
	onfly.src_block_w = 36;
	onfly.src_block_h = 35;
	onfly.jpeg_out_y = 32;
	onfly.jpeg_out_x = 32;
	onfly.fifo_full_cnt = onfly.src_block_w * 2 + 2;
	onfly.in_length = onfly.jpeg_out_x + 3;
	onfly.fifo_data_cnt = onfly.src_block_w * onfly.src_block_h / 2;
	onfly.in_height = onfly.jpeg_out_y + 2;

	GLAMO_DMA_BEGIN(dma, 10);
	GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_ONFLY_MODE1, onfly.src_block_y << 10 | onfly.src_block_x << 2);
	GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_ONFLY_MODE2, onfly.src_block_h << 8 | onfly.src_block_w);
	GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_ONFLY_MODE3, onfly.jpeg_out_y << 8 | onfly.jpeg_out_x);
	GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_ONFLY_MODE4, onfly.fifo_full_cnt << 8 | onfly.in_length);
	GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_ONFLY_MODE5, onfly.fifo_data_cnt << 6 | onfly.in_height);

	/* Smedia suggests these values */
	//GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_ONFLY_MODE1, 0x8080);
	//GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_ONFLY_MODE2, 0x2526);
	//GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_ONFLY_MODE3, 0x2020);
	//GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_ONFLY_MODE4, 0x5023);
	//GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_ONFLY_MODE5, 0xafe2);
	
	/* another set of values they suggest */
	//GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_ONFLY_MODE1, 0x8080);
	//GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_ONFLY_MODE2, 0x2324);
	//GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_ONFLY_MODE3, 0x2020);
	//GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_ONFLY_MODE4, 0x4a23);
	//GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_ONFLY_MODE5, 0x9da2);

	GLAMO_DMA_END(dma);
}

static void blit_set_weight_regs(struct glamo_dma_manager *dma)
{
	int left = 1 << 14;
	GLAMO_DMA_VARS;

	/* nearest */

	GLAMO_DMA_BEGIN(dma, 12);
	GLAMO_DMA_BURST(dma, GLAMO_REG_ISP_DEC_SCALEH_MATRIX, 10);
	GLAMO_DMA_OUT(dma, left, 0);
	GLAMO_DMA_OUT(dma, left, 0);
	GLAMO_DMA_OUT(dma, left, 0);
	GLAMO_DMA_OUT(dma, left, 0);
	GLAMO_DMA_OUT(dma, left, 0);
	GLAMO_DMA_END(dma);

	GLAMO_DMA_BEGIN(dma, 12);
	GLAMO_DMA_BURST(dma, GLAMO_REG_ISP_DEC_SCALEV_MATRIX, 10);
	GLAMO_DMA_OUT(dma, left, 0);
	GLAMO_DMA_OUT(dma, left, 0);
	GLAMO_DMA_OUT(dma, left, 0);
	GLAMO_DMA_OUT(dma, left, 0);
	GLAMO_DMA_OUT(dma, left, 0);
	GLAMO_DMA_END(dma);
}

struct glamo_blit_manager *glamo_blit_new(struct glamo_dma_manager *dma,
		enum glamo_blit_format format)
{
	struct glamo_blit_manager *blit;
	int en3;
	GLAMO_DMA_VARS;

	blit = malloc(sizeof(*blit));
	if (!blit)
		return NULL;

	blit->dma = dma;
	blit->format = format;

	glamo_hw_engine_enable(GLAMO_ENGINE_ISP);
	glamo_hw_engine_reset(GLAMO_ENGINE_ISP);

	switch (blit->format)
	{
	case GLAMO_BLIT_FORMAT_I420:
		en3 = GLAMO_ISP_EN3_PLANE_MODE |
		      GLAMO_ISP_EN3_YUV_INPUT |
		      GLAMO_ISP_EN3_YUV420;
		break;
	case GLAMO_BLIT_FORMAT_YV16:
		en3 = GLAMO_ISP_EN3_PLANE_MODE |
		      GLAMO_ISP_EN3_YUV_INPUT;
		break;
	case GLAMO_BLIT_FORMAT_YUY2:
		en3 = GLAMO_ISP_EN3_YUV_INPUT;
		break;
	}
	en3 |= GLAMO_ISP_EN3_SCALE_IMPROVE;

	GLAMO_DMA_BEGIN(dma, 18);

	/*
	 * In 8.8 fixed point,
	 *
	 *  R = Y + 1.402 (Cr-128)
	 *    = Y + 0x0167 Cr - 0xb3
	 *
	 *  G = Y - 0.34414 (Cb-128) - 0.71414 (Cr-128)
	 *    = Y - 0x0058 Cb - 0x00b6 Cr + 0x89
	 *
	 *  B = Y + 1.772 (Cb-128)
	 *    = Y + 0x01c5 Cb - 0xe2
	 */
	GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_YUV2RGB_11, 0x0167);
	GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_YUV2RGB_21, 0x01c5);
	GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_YUV2RGB_32, 0x00b6);
	GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_YUV2RGB_33, 0x0058);
	GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_YUV2RGB_RG, 0xb3 << 8 | 0x89);
	GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_YUV2RGB_B, 0xe2);

	GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_EN3, en3);

	GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_PORT1_DEC_EN, GLAMO_ISP_PORT1_EN_OUTPUT);
	GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_PORT2_EN, GLAMO_ISP_PORT2_EN_DECODE);

	GLAMO_DMA_END(dma);

	blit_set_onfly_regs(dma);
	blit_set_weight_regs(dma);

	return blit;
}

void glamo_blit_rotate(struct glamo_blit_manager *blit, 
		enum glamo_blit_rotation rot)
{
	int en4;
	GLAMO_DMA_VARS;

	switch (rot)
	{
	case GLAMO_BLIT_ROTATION_0:
		en4 = GLAMO_ISP_ROT_MODE_0;
		break;
	case GLAMO_BLIT_ROTATION_90:
		en4 = GLAMO_ISP_ROT_MODE_90;
		break;
	case GLAMO_BLIT_ROTATION_270:
		en4 = GLAMO_ISP_ROT_MODE_270;
		break;
	case GLAMO_BLIT_ROTATION_180:
		en4 = GLAMO_ISP_ROT_MODE_180;
		break;
	case GLAMO_BLIT_ROTATION_MIRROR:
		en4 = GLAMO_ISP_ROT_MODE_MIRROR;
		break;
	case GLAMO_BLIT_ROTATION_FLIP:
		en4 = GLAMO_ISP_ROT_MODE_FLIP;
		break;
	}

	GLAMO_DMA_BEGIN(blit->dma, 2);
	GLAMO_DMA_OUT(blit->dma, GLAMO_REG_ISP_EN4, en4);
	GLAMO_DMA_END(blit->dma);
}

void glamo_blit_destroy(struct glamo_blit_manager *blit)
{
	glamo_hw_engine_reset(GLAMO_ENGINE_ISP);
	glamo_hw_engine_disable(GLAMO_ENGINE_ISP);

	free(blit);
}

void glamo_blit_wait(struct glamo_blit_manager *blit)
{
	while (1)
	{
		int val = GLAMO_IN_REG(GLAMO_REG_ISP_STATUS);
		if (val & 0x1)
		{
			usleep(1 * 1000);
			printf("isp busy\n");

			continue;
		}

		break;
	}
}

void glamo_blit_dump(struct glamo_blit_manager *blit)
{
	glamo_hw_dump(GLAMO_REG_ISP_EN1, (GLAMO_REG_ISP_STATUS - GLAMO_REG_ISP_EN1) / 2 + 1);
}

struct glamo_blit_image *glamo_blit_new_image(struct glamo_blit_manager *blit,
		const unsigned char *data, int width, int height)
{
	struct glamo_blit_image *image;

	switch (blit->format)
	{
	case GLAMO_BLIT_FORMAT_I420:
		if (width & 1 || height & 1)
			return NULL;
		break;
	case GLAMO_BLIT_FORMAT_YV16:
		if (width & 1 || height & 1)
			return NULL;
		break;
	case GLAMO_BLIT_FORMAT_YUY2:
		break;
	}

	image = malloc(sizeof(*image));
	if (!image)
		return NULL;

	image->format = blit->format;
	image->data = data;
	image->width = width;
	image->height = height;

	switch (image->format)
	{
	case GLAMO_BLIT_FORMAT_I420:
		image->num_planes = 3;

		image->pitches[0] = width;
		image->offsets[0] = 0;

		image->pitches[1] = width / 2;
		image->offsets[1] = image->offsets[0] + image->pitches[0] * image->height;

		image->pitches[2] = width / 2;
		image->offsets[2] = image->offsets[1] + image->pitches[1] * image->height / 2;
		break;
	case GLAMO_BLIT_FORMAT_YV16:
		image->num_planes = 3;

		image->pitches[0] = width;
		image->offsets[0] = 0;

		image->pitches[1] = width / 2;
		image->offsets[1] = image->offsets[0] + image->pitches[0] * image->height;

		image->pitches[2] = width / 2;
		image->offsets[2] = image->offsets[1] + image->pitches[1] * image->height;
		break;
	case GLAMO_BLIT_FORMAT_YUY2:
		image->num_planes = 1;
		image->pitches[0] = width * 2;
		image->offsets[0] = 0;
		break;
	}

	return image;
}

void glamo_blit_destroy_image(struct glamo_blit_manager *blit, struct glamo_blit_image *image)
{
	free(image);
}

void glamo_blit_dump_image(struct glamo_blit_manager *blit, struct glamo_blit_image *image)
{
	int i;

	printf("dumping image\n"
	       "format 0x%x\n"
	       "data 0x%x\n"
	       "width %d\n"
	       "height %d\n"
	       "num_planes %d\n",
	       image->format,
	       (unsigned int) image->data,
	       image->width,
	       image->height,
	       image->num_planes);

	for (i = 0; i < image->num_planes; i++)
	{
		printf("pitches[%d] %d\n"
		       "offsets[%d] %d\n",
		       i, image->pitches[i],
		       i, image->offsets[i]);
	}
}

static void blit_image_regs(struct glamo_dma_manager *dma, struct glamo_blit_image *image)
{
	unsigned int offset;
	GLAMO_DMA_VARS;

	switch (image->format)
	{
	case GLAMO_BLIT_FORMAT_I420:
	case GLAMO_BLIT_FORMAT_YV16:
		GLAMO_DMA_BEGIN(dma, 20);

		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_DEC_PITCH_Y, image->pitches[0]);
		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_DEC_PITCH_UV, image->pitches[1]);

		offset = (unsigned int) image->data + image->offsets[0];
		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_DEC_Y_ADDRL, offset & 0xffff);
		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_DEC_Y_ADDRH, (offset >> 16) & 0x7f);

		offset = (unsigned int) image->data + image->offsets[1];
		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_DEC_U_ADDRL, offset & 0xffff);
		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_DEC_U_ADDRH, (offset >> 16) & 0x7f);

		offset = (unsigned int) image->data + image->offsets[2];
		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_DEC_V_ADDRL, offset & 0xffff);
		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_DEC_V_ADDRH, (offset >> 16) & 0x7f);

		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_DEC_HEIGHT, image->height);
		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_DEC_WIDTH, image->width);

		GLAMO_DMA_END(dma);
		break;
	case GLAMO_BLIT_FORMAT_YUY2:
		GLAMO_DMA_BEGIN(dma, 10);

		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_CAP_PITCH, image->pitches[0]);

		offset = (unsigned int) image->data + image->offsets[0];
		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_CAP_0_ADDRL, offset & 0xffff);
		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_CAP_0_ADDRH, (offset >> 16) & 0x7f);

		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_CAP_HEIGHT, image->height);
		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_CAP_WIDTH, image->width);

		GLAMO_DMA_END(dma);
		break;
	}
}

static void blit_port_regs(struct glamo_dma_manager *dma, struct glamo_blit_image *image,
		int x, int y, int width, int height)
{
	unsigned int offset, scale;
	GLAMO_DMA_VARS;

	offset = y * glamo_pitch + x * 2;

	switch (image->format)
	{
	case GLAMO_BLIT_FORMAT_I420:
	case GLAMO_BLIT_FORMAT_YV16:
		GLAMO_DMA_BEGIN(dma, 14);

		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_PORT1_DEC_PITCH, glamo_pitch);

		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_PORT1_DEC_0_ADDRL, offset & 0xffff);
		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_PORT1_DEC_0_ADDRH, (offset >> 16) & 0x7f);

		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_PORT1_DEC_WIDTH, width);
		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_PORT1_DEC_HEIGHT, height);

		scale = (image->width << 11) / width;
		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_DEC_SCALEH, scale);

		scale = (image->height << 11) / height;
		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_DEC_SCALEV, scale);

		GLAMO_DMA_END(dma);
		break;
	case GLAMO_BLIT_FORMAT_YUY2:
		GLAMO_DMA_BEGIN(dma, 14);

		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_PORT1_CAP_PITCH, glamo_pitch);

		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_PORT1_CAP_0_ADDRL, offset & 0xffff);
		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_PORT1_CAP_0_ADDRH, (offset >> 16) & 0x7f);

		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_PORT1_CAP_WIDTH, width);
		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_PORT1_CAP_HEIGHT, height);

		scale = (image->width << 11) / width;
		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_DEC_SCALEH, scale);

		scale = (image->height << 11) / height;
		GLAMO_DMA_OUT(dma, GLAMO_REG_ISP_DEC_SCALEV, scale);

		GLAMO_DMA_END(dma);
		break;
	}
}

void glamo_blit_show_image(struct glamo_blit_manager *blit, struct glamo_blit_image *image,
		int x, int y, int width, int height)
{
	blit_image_regs(blit->dma, image);
	blit_port_regs(blit->dma, image, x, y, width, height);

	glamo_dma_flush(blit->dma);
	glamo_dma_wait(blit->dma, GLAMO_DMA_WAIT_CMDQ);

	glamo_set_bit_mask(GLAMO_REG_ISP_EN1, GLAMO_ISP_EN1_FIRE_ISP, 0xffff);
	glamo_set_bit_mask(GLAMO_REG_ISP_EN1, GLAMO_ISP_EN1_FIRE_ISP, 0);
}
