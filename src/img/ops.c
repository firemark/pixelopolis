#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <png.h>
#include <limits.h>

#include "pixelopolis/img/ops.h"

#define _GET_INDEX(img, x, y) ( \
    (img->height - 1 - y) * img->width + x \
)
#define _INIT(img, filler) \
    const int width = img->width; \
    const int height = img->height; \
    const int filler_width = filler->width; \
    const int filler_height = filler->height
#define _LIMIT(val, filler_dim, dim) ( \
    val + filler_dim < dim ? filler_dim : dim - val \
)

#define _FILL(img, filler) {\
    _INIT(img, filler); \
    int x, y; \
    for (y=0; y < height; y++) { \
        for(x=0; x < width; x++) { \
            const size_t filler_index = _GET_INDEX(filler, x % filler_width, y % filler_height); \
            const size_t img_index = _GET_INDEX(img, x, y); \
            img->buffer[img_index] = filler->buffer[filler_index]; \
        } \
    } \
}

#define _FILL_COLUMN(img, filler, img_y) { \
    _INIT(img, filler); \
    const int limit_height = _LIMIT(img_y, filler_height, height); \
    int x, y; \
    for (y=0; y < limit_height; y++) { \
        for(x=0; x < width; x++) { \
            const size_t filler_index = _GET_INDEX(filler, x % filler_width, y); \
            const size_t img_index = _GET_INDEX(img, x, img_y); \
            img->buffer[img_index] = filler->buffer[filler_index]; \
        } \
        img_y++; \
    } \
}

#define _COPY(img, filler, img_x, img_y) { \
    _INIT(img, filler); \
    const int limit_width = _LIMIT(img_y, filler_width, width); \
    const int limit_height = _LIMIT(img_y, filler_height, height); \
    int x, y, tmp_img_x; \
    for (y=0; y < limit_height; y++) { \
        tmp_img_x = img_x; \
        for(x=0; x < limit_width; x++) { \
            size_t filler_index = _GET_INDEX(filler, x, y); \
            size_t img_index = _GET_INDEX(img, tmp_img_x, img_y); \
            img->buffer[img_index] = filler->buffer[filler_index]; \
            tmp_img_x++; \
        } \
        img_y++; \
    } \
}

void flat_image_fill(struct FlatImage* img, const struct FlatImage* filler) {
    _FILL(img, filler);
}

void flat_image_fill_column(struct FlatImage* img, const struct FlatImage* filler, int img_y) {
    _FILL_COLUMN(img, filler, img_y);
}

void flat_image_copy(struct FlatImage* img, const struct FlatImage* filler, int img_x, int img_y) {
    _COPY(img, filler, img_x, img_y);
}

void float_image_fill(struct FloatImage* img, const struct FloatImage* filler) {
    _FILL(img, filler);
}

void float_image_fill_column(struct FloatImage* img, const struct FloatImage* filler, int img_y) {
    _FILL_COLUMN(img, filler, img_y);
}

void float_image_copy(struct FloatImage* img, const struct FloatImage* filler, int img_x, int img_y) {
    _COPY(img, filler, img_x, img_y);
}
