#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>

#include "hw.h"

#define FB_SIZE 0x800000

static int fd_mem;
static int fd_fb;
static struct fb_fix_screeninfo fix;

volatile unsigned char *glamo_fb;
volatile unsigned char *glamo_mmio;
int glamo_pitch;

void glamo_os_init(void)
{
	fd_fb = open("/dev/fb0", O_RDWR | O_SYNC);
	if (fd_fb < 0)
	{
		printf("error open fb0\n");
		exit(1);
	}

	if (ioctl(fd_fb, FBIOGET_FSCREENINFO, &fix) < 0)
	{
		printf("error FBIOGET_FSCREENINFO\n");

		close(fd_fb);
		exit(1);
	}

	/* hard-coded */
	fix.smem_len = FB_SIZE;
	glamo_pitch = fix.line_length;

	fd_mem = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd_mem < 0)
	{
		printf("error open mem\n");

		exit(1);
	}

	glamo_fb = mmap(NULL, fix.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd_mem, fix.smem_start);
	if (glamo_fb == MAP_FAILED)
	{
		printf("error mmap fb\n");
		close(fd_mem);

		exit(1);
	}

	glamo_mmio = mmap(NULL, GLAMO_MMIO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_mem, GLAMO_MMIO_BASE);
	if (glamo_mmio == MAP_FAILED)
	{
		printf("error mmap mmio\n");

		munmap((void *) glamo_fb, fix.smem_len);
		close(fd_mem);

		exit(1);
	}
#if 0

	printf("fb   0x%08lx (0x%x)\n"
	       "mmio 0x%08x (0x%x)\n",
	       fix.smem_start, fix.smem_len,
	       GLAMO_MMIO_BASE, GLAMO_MMIO_SIZE);
#endif
}

void glamo_os_finish(void)
{
	munmap((void *) glamo_mmio, GLAMO_MMIO_SIZE);
	munmap((void *) glamo_fb, fix.smem_len);

	close(fd_fb);
	close(fd_mem);
}
