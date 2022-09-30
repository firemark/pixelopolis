#include "pixelopolis/img/create.h"

#include <limits.h>
#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define _CREATE(img, width, height, image_type)                         \
    IMG_TYPE_##image_type* img = malloc(sizeof(IMG_TYPE_##image_type)); \
    img->width = width;                                                 \
    img->height = height;                                               \
    img->buffer = malloc(width * height * sizeof(PIXEL_TYPE_##image_type))

struct image* create_black_image(const int width, const int height) {
    _CREATE(img, width, height, IMAGE);
    int x, y;
    for (y = 0; y < img->height; y++) {
        for (x = 0; x < img->width; x++) {
            struct RoyalPixel pixel = {
                .r = 0,
                .g = 0,
                .b = 0,
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

struct FloatImage* float_image_create(const int width, const int height) {
    _CREATE(img, width, height, FLOAT_IMAGE);
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
