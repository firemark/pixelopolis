#include "pixelopolis/img/create.h"

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define _SET_DIMS(img, width, height) \
    img->width = width;               \
    img->height = height

#define _CREATE(img, width, height, image_type)                             \
    IMG_TYPE_##image_type* img = malloc(sizeof(IMG_TYPE_##image_type));     \
    img->buffer = malloc(width * height * sizeof(PIXEL_TYPE_##image_type)); \
    _SET_DIMS(img, width, height)

#define _CREATE_MEMORY(memory, img, width, height, image_type)                            \
    IMG_TYPE_##image_type* img = MEMORY_ALLOCATE(memory, IMG_TYPE_##image_type);          \
    img->buffer = MEMORY_ALLOCATE_ARRAY(memory, PIXEL_TYPE_##image_type, width * height); \
    _SET_DIMS(img, width, height)

struct image* create_black_image(const int width, const int height) {
    _CREATE(img, width, height, IMAGE);
    int x, y;
    for (y = 0; y < img->height; y++) {
        for (x = 0; x < img->width; x++) {
            struct RoyalPixel pixel = {
                .r = 0,
                .g = 0,
                .b = 0,
                .x = 0.5f,
                .y = 0.5f,
                .z = 1.0f,
                .zindex = INT_MAX,
            };
            img->buffer[y * img->width + x] = pixel;
        }
    }
    return img;
}

struct FlatImage* flat_image_create(const int width, const int height) {
    _CREATE(img, width, height, FLAT_IMAGE);
    return img;
}

struct FlatImage* flat_image_create_memory(struct Memory* memory, const int width,
                                           const int height) {
    _CREATE_MEMORY(memory, img, width, height, FLAT_IMAGE);
    return img;
}

struct FlatImage* flat_image_create_with_color(const int width, const int height,
                                               const struct rgb* color) {
    struct FlatImage* img = flat_image_create(width, height);
    struct rgb c = *color;
    int x, y;
    for (y = 0; y < img->height; y++) {
        for (x = 0; x < img->width; x++) {
            img->buffer[y * width + x] = c;
        }
    }
    return img;
}

struct OneChanImage* one_chan_image_create(const int width, const int height) {
    _CREATE(img, width, height, ONE_CHAN_IMAGE);
    return img;
}

struct OneChanImage* one_chan_image_create_memory(struct Memory* memory, const int width,
                                                  const int height) {
    _CREATE_MEMORY(memory, img, width, height, ONE_CHAN_IMAGE);
    return img;
}

struct OneChanImage* one_chan_image_create_with_color(const int width, const int height,
                                                      uint8_t color) {
    struct OneChanImage* img = one_chan_image_create(width, height);
    int x, y;
    for (y = 0; y < img->height; y++) {
        for (x = 0; x < img->width; x++) {
            img->buffer[y * width + x] = color;
        }
    }
    return img;
}

struct FloatImage* float_image_create(const int width, const int height) {
    _CREATE(img, width, height, FLOAT_IMAGE);
    return img;
}

struct FloatImage* float_image_create_memory(struct Memory* memory, const int width,
                                             const int height) {
    _CREATE_MEMORY(memory, img, width, height, FLOAT_IMAGE);
    return img;
}

struct FloatImage* float_image_create_with_color(const int width, const int height,
                                                 const struct xyz* color) {
    struct FloatImage* img = float_image_create(width, height);
    struct xyz c = *color;
    int x, y;
    for (y = 0; y < img->height; y++) {
        for (x = 0; x < img->width; x++) {
            img->buffer[y * width + x] = c;
        }
    }
    return img;
}
