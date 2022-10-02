#include "pixelopolis/img/ops.h"

#include <limits.h>
#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define _INIT(img, filler)                  \
    const int width = img->width;           \
    const int height = img->height;         \
    const int filler_width = filler->width; \
    const int filler_height = filler->height
#define _LIMIT(val, filler_dim, dim) (val + filler_dim < dim ? filler_dim : dim - val)

#define _FILL(img, filler)                                                                     \
    {                                                                                          \
        _INIT(img, filler);                                                                    \
        int x, y;                                                                              \
        for (y = 0; y < height; y++) {                                                         \
            for (x = 0; x < width; x++) {                                                      \
                IMG_PIXEL(img, x, y) = IMG_PIXEL(filler, x % filler_width, y % filler_height); \
            }                                                                                  \
        }                                                                                      \
    }

#define _FILL_COLUMN(img, filler, img_y)                                           \
    {                                                                              \
        _INIT(img, filler);                                                        \
        const int limit_height = _LIMIT(img_y, filler_height, height);             \
        int x, y;                                                                  \
        for (y = 0; y < limit_height; y++) {                                       \
            for (x = 0; x < width; x++) {                                          \
                IMG_PIXEL(img, x, img_y) = IMG_PIXEL(filler, x % filler_width, y); \
            }                                                                      \
            img_y++;                                                               \
        }                                                                          \
    }

#define _COPY(img, filler, img_x, img_y)                                    \
    {                                                                       \
        _INIT(img, filler);                                                 \
        const int limit_width = _LIMIT(img_y, filler_width, width);         \
        const int limit_height = _LIMIT(img_y, filler_height, height);      \
        int x, y, tmp_img_x;                                                \
        for (y = 0; y < limit_height; y++) {                                \
            tmp_img_x = img_x;                                              \
            for (x = 0; x < limit_width; x++) {                             \
                IMG_PIXEL(img, tmp_img_x, img_y) = IMG_PIXEL(filler, x, y); \
                tmp_img_x++;                                                \
            }                                                               \
            img_y++;                                                        \
        }                                                                   \
    }

void flat_image_fill(struct FlatImage* img, const struct FlatImage* filler) { _FILL(img, filler); }

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

void float_image_copy(struct FloatImage* img, const struct FloatImage* filler, int img_x,
                      int img_y) {
    _COPY(img, filler, img_x, img_y);
}
