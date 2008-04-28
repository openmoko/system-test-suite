#ifndef _DMA_H_
#define _DMA_H_

#include <stdlib.h> /* for exit() */
#include "hw.h"

#define GLAMO_REG_WIDTH sizeof(GLAMO_REG_TYPE)

/*
 * some engines only support MMIO mode; while
 * some engines only support CMDQ mode
 */
enum glamo_dma_mode {
	GLAMO_DMA_MODE_MMIO,
	GLAMO_DMA_MODE_CMDQ,
};

enum glamo_dma_wait_type {
	GLAMO_DMA_WAIT_CMDQ,
	GLAMO_DMA_WAIT_ISP,
	GLAMO_DMA_WAIT_ALL,
};

struct glamo_dma_manager {
	enum glamo_dma_mode mode;

	unsigned char *lbuf;
	int lsize;
	int llen;

	volatile GLAMO_REG_TYPE *cmdq;
	int size;
	int r;
	int w;
};

struct glamo_dma_manager *glamo_dma_new(enum glamo_dma_mode t);
void glamo_dma_destroy(struct glamo_dma_manager *dma);
void glamo_dma_flush(struct glamo_dma_manager *dma);
void glamo_dma_wait(struct glamo_dma_manager *dma, enum glamo_dma_wait_type t);
void glamo_dma_dump(struct glamo_dma_manager *dma);

#define GLAMO_DMA_VARS	\
	GLAMO_REG_TYPE *__p; int __count, __total

#define GLAMO_DMA_BEGIN(dma, n)						\
	do {								\
		if (dma->llen + n * GLAMO_REG_WIDTH > dma->lsize)	\
			glamo_dma_flush(dma);				\
		__p = (GLAMO_REG_TYPE *) (dma->lbuf + dma->llen);	\
		__count = 0; __total = n;				\
	} while (0)

#define GLAMO_DMA_OUT(dma, v1, v2)	\
	do {				\
		__p[__count++] = v1;	\
		__p[__count++] = v2;	\
	} while (0)

#define GLAMO_DMA_END(dma)						\
	do {								\
		if (__count != __total)					\
		{							\
			fprintf(stderr, "count != total (%d != %d) "	\
					"at %s:%d\n", __count, __total,	\
					__FILE__, __LINE__);		\
			exit(1);					\
		}							\
		dma->llen += __count * GLAMO_REG_WIDTH;			\
	} while (0)

#define GLAMO_DMA_BURST(dma, reg, n)		\
	GLAMO_DMA_OUT(dma, (1 << 15) | reg, n)

#endif /* _DMA_H */
