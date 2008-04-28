#ifndef _GLAMO_H_
#define _GLAMO_H_

#define GLAMO_OUT_REG(reg, val) (*((volatile unsigned short *) ((glamo_mmio) + (reg))) = (val))
#define GLAMO_IN_REG(reg) (*((volatile unsigned short *) ((glamo_mmio) + (reg))))

extern volatile unsigned char *glamo_fb;
extern volatile unsigned char *glamo_mmio;
extern int glamo_pitch;

inline static void glamo_set_bit_mask(int reg, int mask, int val)
{
	int old;

	old = GLAMO_IN_REG(reg);
	old &= ~mask;
	old |= val & mask;
	GLAMO_OUT_REG(reg, old);
}

void glamo_os_init(void);
void glamo_os_finish(void);

#endif /* _GLAMO_H_ */
