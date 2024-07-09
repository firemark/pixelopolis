#include "pixelopolis/img/ops.h"

#include <stdio.h>
#include <unistd.h>

#define _GET_INDEX(img, x, y) ((img->height - 1 - y) * img->width + x)
#define _LIMIT(val, filler_dim, dim) (val + filler_dim < dim ? filler_dim : dim - val)

#define _FILL(img, filler)                                                     \
    {                                                                          \
        int x, y;                                                              \
        for (y = 0; y < img->height; y++) {                                    \
            for (x = 0; x < img->width; x++) {                                 \
                const size_t filler_index =                                    \
                    _GET_INDEX(filler, x % filler->width, y % filler->height); \
                const size_t img_index = _GET_INDEX(img, x, y);                \
                img->buffer[img_index] = filler->buffer[filler_index];         \
            }                                                                  \
        }                                                                      \
    }

#define _FILL_COLUMN(img, filler, img_y)                                              \
    {                                                                                 \
        const int limit_height = _LIMIT(img_y, filler->height, img->height);          \
        int x, y;                                                                     \
        for (y = 0; y < limit_height; y++) {                                          \
            for (x = 0; x < img->width; x++) {                                        \
                const size_t filler_index = _GET_INDEX(filler, x % filler->width, y); \
                const size_t img_index = _GET_INDEX(img, x, img_y);                   \
                img->buffer[img_index] = filler->buffer[filler_index];                \
            }                                                                         \
            img_y++;                                                                  \
        }                                                                             \
    }

#define _COPY_COND(img, filler, img_x, img_y, __cond__)                      \
    {                                                                        \
        const int limit_width = _LIMIT(img_x, filler->width, img->width);    \
        const int limit_height = _LIMIT(img_y, filler->height, img->height); \
        int old_img_x = img_x;                                               \
        int x, y;                                                            \
        for (y = 0; y < limit_height; y++) {                                 \
            for (x = 0; x < limit_width; x++) {                              \
                size_t filler_index = _GET_INDEX(filler, x, y);              \
                size_t img_index = _GET_INDEX(img, img_x, img_y);            \
                img_x++;                                                     \
                __cond__;                                                    \
                img->buffer[img_index] = filler->buffer[filler_index];       \
            }                                                                \
            img_x = old_img_x;                                               \
            img_y++;                                                         \
        }                                                                    \
    }

#define _COPY(img, filler, img_x, img_y) _COPY_COND(img, filler, img_x, img_y, {})

#define _COPY_TRANSPARENT(img, filler, mask, img_x, img_y)                                      \
    _COPY_COND(img, filler, img_x, img_y, {                                                     \
        const struct rgb* mask_byte = &mask->buffer[filler_index];                              \
        if (mask_byte->r == PURPLE.r && mask_byte->g == PURPLE.g && mask_byte->b == PURPLE.b) { \
            continue;                                                                           \
        }                                                                                       \
    })

void flat_image_fill(struct FlatImage* img, const struct FlatImage* filler) { _FILL(img, filler); }

void flat_image_fill_column(struct FlatImage* img, const struct FlatImage* filler, int img_y) {
    _FILL_COLUMN(img, filler, img_y);
}

void flat_image_copy(struct FlatImage* img, const struct FlatImage* filler, int img_x, int img_y) {
    _COPY(img, filler, img_x, img_y);
}

void flat_image_copy_transparent(struct FlatImage* img, const struct FlatImage* filler,
                                 const struct FlatImage* mask, int img_x, int img_y) {
    _COPY_TRANSPARENT(img, filler, mask, img_x, img_y);
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

void float_image_copy_transparent(struct FloatImage* img, const struct FloatImage* filler,
                                  const struct FlatImage* mask, int img_x, int img_y) {
    _COPY_TRANSPARENT(img, filler, mask, img_x, img_y);
}