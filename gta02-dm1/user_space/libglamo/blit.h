#ifndef _BLIT_H_
#define _BLIT_H_

#include "dma.h"

enum glamo_blit_format {
	GLAMO_BLIT_FORMAT_I420,
	GLAMO_BLIT_FORMAT_YV16,
	GLAMO_BLIT_FORMAT_YUY2,
};

enum glamo_blit_rotation {
	GLAMO_BLIT_ROTATION_0,
	GLAMO_BLIT_ROTATION_90,
	GLAMO_BLIT_ROTATION_270,
	GLAMO_BLIT_ROTATION_180,
	GLAMO_BLIT_ROTATION_MIRROR,
	GLAMO_BLIT_ROTATION_FLIP
};

struct glamo_blit_image {
	enum glamo_blit_format format;
	const unsigned char *data;
	int width;
	int height;
	int num_planes; /* 1 or 3 */
	int pitches[3];
	int offsets[3];
};

struct glamo_blit_manager {
	struct glamo_dma_manager *dma;
	enum glamo_blit_format format;
};

struct glamo_blit_manager *glamo_blit_new(struct glamo_dma_manager *dma, enum glamo_blit_format format);
void glamo_blit_destroy(struct glamo_blit_manager *blit);
void glamo_blit_wait(struct glamo_blit_manager *blit);
void glamo_blit_dump(struct glamo_blit_manager *blit);

void glamo_blit_rotate(struct glamo_blit_manager *blit, 
		enum glamo_blit_rotation);

struct glamo_blit_image *glamo_blit_new_image(struct glamo_blit_manager *blit,
		const unsigned char *data, int width, int height);
void glamo_blit_destroy_image(struct glamo_blit_manager *blit, struct glamo_blit_image *image);
void glamo_blit_show_image(struct glamo_blit_manager *blit, struct glamo_blit_image *image,
		int x, int y, int width, int height);
void glamo_blit_dump_image(struct glamo_blit_manager *blit, struct glamo_blit_image *image);

#endif /* _BLIT_H */
