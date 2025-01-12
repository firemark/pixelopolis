#pragma once
#include <stdint.h>

#define COPY_VOX(vox) \
    { vox[0], vox[1], vox[2] }
#define ZERO_VOX \
    { 0, 0, 0 }

#define IMG_TYPE_IMAGE struct image
#define PIXEL_TYPE_IMAGE struct RoyalPixel

#define IMG_TYPE_FLAT_IMAGE struct FlatImage
#define PIXEL_TYPE_FLAT_IMAGE struct rgb

#define IMG_TYPE_ONE_CHAN_IMAGE struct OneChanImage
#define PIXEL_TYPE_ONE_CHAN_IMAGE uint8_t

#define IMG_TYPE_FLOAT_IMAGE struct FloatImage
#define PIXEL_TYPE_FLOAT_IMAGE struct xyz

struct rgb {
    uint8_t r, g, b;
};

struct RoyalPixel {
    uint8_t r, g, b;
    float x, y, z;
    int zindex;
};
struct hsv {
    uint8_t h, s, v;
};
struct xyz {
    float x, y, z;
};
enum direction { DIRECTION_UP, DIRECTION_EAST, DIRECTION_SOUTH };

static const struct rgb PURPLE = {.r = 0xFF, .g = 0x00, .b = 0xFF};
static const struct xyz FORWARD = {.x = 0.0f, .y = 0.0f, .z = 1.0f};

#define _DEFINE_IMAGE(image_type)        \
    IMG_TYPE_##image_type {              \
        PIXEL_TYPE_##image_type* buffer; \
        int width;                       \
        int height;                      \
    }

_DEFINE_IMAGE(IMAGE);
_DEFINE_IMAGE(FLAT_IMAGE);
_DEFINE_IMAGE(FLOAT_IMAGE);
_DEFINE_IMAGE(ONE_CHAN_IMAGE);

#undef _DEFINE_IMAGE
