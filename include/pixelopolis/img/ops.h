#pragma once
#include "../basic.h"

void flat_image_fill(struct FlatImage* img, const struct FlatImage* filler);
void flat_image_fill_column(struct FlatImage* img, const struct FlatImage* filler, int img_y);
void flat_image_copy(struct FlatImage* img, const struct FlatImage* filler, int img_x, int img_y);
void float_image_fill(struct FloatImage* img, const struct FloatImage* filler);
void float_image_fill_column(struct FloatImage* img, const struct FloatImage* filler, int img_y);
void float_image_copy(struct FloatImage* img, const struct FloatImage* filler, int img_x, int img_y);

#define IMG_INDEX(img, x, y) ((img->height - 1 - (y)) * img->width + (x))
#define IMG_PIXEL(img, x, y) img->buffer[IMG_INDEX(img, x, y)]