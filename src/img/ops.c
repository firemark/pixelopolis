#include "pixelopolis/img/ops.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define _GET_INDEX(img, x, y) ((img->height - 1 - y) * img->width + x)
#define _LIMIT(val, filler_dim, dim) (val + filler_dim < dim ? filler_dim : dim - val)

#define _FILL_COND(img, filler, __cond__)                                      \
    {                                                                          \
        int x, y;                                                              \
        for (y = 0; y < img->height; y++) {                                    \
            for (x = 0; x < img->width; x++) {                                 \
                const size_t filler_index =                                    \
                    _GET_INDEX(filler, x % filler->width, y % filler->height); \
                const size_t img_index = _GET_INDEX(img, x, y);                \
                __cond__;                                                      \
                img->buffer[img_index] = filler->buffer[filler_index];         \
            }                                                                  \
        }                                                                      \
    }

#define _FILL(img, filler) _FILL_COND(img, filler, {})

#define _FILL_TRANSPARENT(img, filler, mask)                                                    \
    _FILL_COND(img, filler, {                                                                   \
        const struct rgb* mask_byte = &mask->buffer[filler_index];                              \
        if (mask_byte->r == PURPLE.r && mask_byte->g == PURPLE.g && mask_byte->b == PURPLE.b) { \
            continue;                                                                           \
        }                                                                                       \
    })

#define _COPY_COND(img, filler, img_x, img_y, __cond__)                      \
    {                                                                        \
        const int limit_width = _LIMIT(img_x, filler->width, img->width);    \
        const int limit_height = _LIMIT(img_y, filler->height, img->height); \
        const int x_start = img_x > 0 ? 0 : -img_x;                          \
        const int y_start = img_y > 0 ? 0 : -img_y;                          \
        img_x += x_start;                                                    \
        img_y += y_start;                                                    \
        int old_img_x = img_x;                                               \
        int x, y;                                                            \
        for (y = y_start; y < limit_height; y++) {                           \
            for (x = x_start; x < limit_width; x++) {                        \
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

#define _CLONE(dst, src, image_type)                       \
    assert(dst->width == src->width);                      \
    assert(dst->height == src->height);                    \
    size_t cells = dst->width * dst->height;               \
    size_t size = cells * sizeof(PIXEL_TYPE_##image_type); \
    memcpy(dst->buffer, src->buffer, size)

void flat_image_fill(struct FlatImage* img, const struct FlatImage* filler) { _FILL(img, filler); }

void flat_image_fill_transparent(struct FlatImage* img, const struct FlatImage* filler,
                                 const struct FlatImage* mask) {
    _FILL_TRANSPARENT(img, filler, mask);
}

void flat_image_copy(struct FlatImage* img, const struct FlatImage* filler, int img_x, int img_y) {
    _COPY(img, filler, img_x, img_y);
}

void flat_image_clone(struct FlatImage* dst, const struct FlatImage* src) {
    _CLONE(dst, src, FLAT_IMAGE);
}

void flat_image_copy_transparent(struct FlatImage* img, const struct FlatImage* filler,
                                 const struct FlatImage* mask, int img_x, int img_y) {
    _COPY_TRANSPARENT(img, filler, mask, img_x, img_y);
}

void float_image_fill(struct FloatImage* img, const struct FloatImage* filler) {
    _FILL(img, filler);
}

void float_image_fill_transparent(struct FloatImage* img, const struct FloatImage* filler,
                                  const struct FlatImage* mask) {
    _FILL_TRANSPARENT(img, filler, mask);
}

void float_image_copy(struct FloatImage* img, const struct FloatImage* filler, int img_x,
                      int img_y) {
    _COPY(img, filler, img_x, img_y);
}

void float_image_copy_transparent(struct FloatImage* img, const struct FloatImage* filler,
                                  const struct FlatImage* mask, int img_x, int img_y) {
    _COPY_TRANSPARENT(img, filler, mask, img_x, img_y);
}

void float_image_clone(struct FloatImage* dst, const struct FloatImage* src) {
    _CLONE(dst, src, FLOAT_IMAGE);
}