#include "dm_init.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <linux/vt.h>
#include <linux/kd.h>

struct framebuffer {
	unsigned char *buf;
	int width;
	int height;
	int pitch;
	int Bbp;

	int fd;
	struct fb_fix_screeninfo fix;
	struct fb_var_screeninfo var;
};

struct console_state {
	int fd;
	int last_vt;
};

static void fb_fill(struct framebuffer *fb, int pixel)
{
	int i, j;
	
	if (fb->Bbp != 2)
		return;

	for (i = 0; i < fb->height; i++)
	{
		unsigned short *line = (unsigned short *) (fb->buf + i * fb->pitch);

		for (j = 0; j < fb->width; j++)
			line[j] = pixel;
	}
}

static int fb_get_pixel(struct framebuffer *fb, int r, int g, int b)
{
	struct fb_var_screeninfo *var = &fb->var;

	r = (r & 0xffff) >> (16 - var->red.length);
	g = (g & 0xffff) >> (16 - var->green.length);
	b = (b & 0xffff) >> (16 - var->blue.length);

	return (r << var->red.offset) | (g << var->green.offset) | (b << var->blue.offset);
}

static struct framebuffer *fb_open(const char *dev)
{
	struct framebuffer *fb;

	fb = malloc(sizeof(*fb));
	if (!fb)
		return NULL;

	fb->fd = open(dev, O_RDWR);
	if (fb->fd < 0)
	{
		perror("failed to open fb");
		free(fb);

		return NULL;
	}

	if (ioctl(fb->fd, FBIOGET_FSCREENINFO, &fb->fix) < 0)
	{
		perror("failed to FBIOGET_FSCREENINFO");
		close(fb->fd);
		free(fb);

		return NULL;
	}

	if (ioctl(fb->fd, FBIOGET_VSCREENINFO, &fb->var) < 0)
	{
		perror("failed to FBIOGET_VSCREENINFO");
		close(fb->fd);
		free(fb);

		return NULL;
	}

	fb->buf = mmap(NULL, fb->fix.smem_len, PROT_WRITE, MAP_SHARED, fb->fd, 0);
	if (fb->buf == MAP_FAILED)
	{
		perror("failed to mmap dev");
		close(fb->fd);
		free(fb);

		return NULL;
	}

	fb->width = fb->var.xres;
	fb->height = fb->var.yres;
	fb->pitch = fb->fix.line_length;
	fb->Bbp = fb->var.bits_per_pixel / 8;

	return fb;
}

static void fb_close(struct framebuffer *fb)
{
	munmap(fb->buf, fb->fix.smem_len);
	close(fb->fd);

	free(fb);
}

static int console_save(struct console_state *cs)
{
	char vtname[128];
	struct vt_stat vts;
	int fd, nr;

	fd = open("/dev/tty0", O_WRONLY);
	if (fd < 0)
	{
		perror("Failed to open /dev/tty0");

		return 0;
	}

	if (ioctl(fd, VT_OPENQRY, &nr) < 0)
	{
		perror("ioctl VT_OPENQRY");
		close(fd);

		return 0;
	}
	close(fd);

	snprintf(vtname, 128, "/dev/tty%d", nr);

	fd = open(vtname, O_RDWR | O_NDELAY);
	if (fd < 0)
	{
		fprintf(stderr, "Failed to open %s: %s\n", vtname, strerror(errno));

		return 0;
	}

	if (ioctl(fd, VT_GETSTATE, &vts) < 0)
	{
		perror("ioctl VT_GETSTATE");
		close(fd);

		return 0;
	}

	if (ioctl(fd, VT_ACTIVATE, nr) < 0)
	{
		perror("ioctl VT_ACTIVATE");
		close(fd);

		return 0;
	}

	if (ioctl(fd, VT_WAITACTIVE, nr) < 0)
	{
		perror("ioctl VT_WAITACTIVE");
		close(fd);

		return 0;
	}

	if (ioctl(fd, KDSETMODE, KD_GRAPHICS) < 0)
	{
		perror("ioctl KDSETMODE");
		close(fd);

		return 0;
	}

	cs->fd = fd;
	cs->last_vt = vts.v_active;

	return 1;
}

static void console_restore(struct console_state *cs)
{
	ioctl(cs->fd, KDSETMODE, KD_TEXT);
	if (ioctl(cs->fd, VT_ACTIVATE, cs->last_vt) == 0)
		ioctl(cs->fd, VT_WAITACTIVE, cs->last_vt);
	close(cs->fd);
}

void lcd_test(void)
{
	struct console_state cs;
	struct framebuffer *fb;
	int pixel;

	if (!console_save(&cs))
	{
		printf("failed to save current console\n");

		return;
	}

	fb = fb_open("/dev/fb0");
	if (!fb)
	{
		printf("failed to open fb\n");
		console_restore(&cs);

		return;
	}

	printf("[LCD] show red\n");
	pixel = fb_get_pixel(fb, 0xffff, 0, 0);
	fb_fill(fb, pixel);
	sleep(2);

	printf("[LCD] show green\n");
	pixel = fb_get_pixel(fb, 0, 0xffff, 0);
	fb_fill(fb, pixel);
	sleep(2);

	printf("[LCD] show blue\n");
	pixel = fb_get_pixel(fb, 0, 0, 0xffff);
	fb_fill(fb, pixel);
	sleep(2);

	fb_close(fb);

	console_restore(&cs);

	return;
}
