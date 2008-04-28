#ifndef _HW_H_
#define _HW_H_

#include "glamo-regs.h"

#define GLAMO_MMIO_BASE 0x8000000
#define GLAMO_MMIO_SIZE 0x2400

#define GLAMO_VRAM_FB	0x000000
#define GLAMO_VRAM_CMDQ	0x100000
#define GLAMO_VRAM_ISP	0x200000
#define GLAMO_VRAM_MPEG	0x300000
#define GLAMO_VRAM_2D	0x400000

enum glamo_engine {
	GLAMO_ENGINE_MPEG,
	GLAMO_ENGINE_ISP,
	GLAMO_ENGINE_CMDQ,
	GLAMO_ENGINE_2D, /* should be last one */
};

typedef unsigned short GLAMO_REG_TYPE;

void glamo_hw_dump(int reg, int len);

void glamo_hw_engine_reset(enum glamo_engine engine);
void glamo_hw_engine_enable(enum glamo_engine engine);
void glamo_hw_engine_disable(enum glamo_engine engine);

#endif /* _HW_H */
