#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <png.h>
#include <limits.h>

#include <img/create.h>

struct image* create_black_image(const int width, const int height) {
    struct image *img = malloc(sizeof(struct image));
    img->width = width;
    img->height = height;
    img->buffer = malloc(width * height * sizeof(struct RoyalPixel));
    int x, y;
    for (y=0; y < img->height; y++) {
        for(x=0; x < img->width; x++) {
            struct RoyalPixel pixel = {
                .r=0,
                .g=0,
                .b=0,
                .zindex=INT_MAX,
            };
            img->buffer[y * img->width + x] = pixel;
        }
    }
    return img;
}

struct FlatImage* flat_image_create(const int width, const int height) {
    struct FlatImage *img = malloc(sizeof(struct FlatImage));
    img->width = width;
    img->height = height;
    img->buffer = malloc(width * height * sizeof(struct rgb));
    return img;
}

struct FlatImage* flat_image_create_with_color(const int width, const int height, const struct rgb* color) {
    struct rgb c = *color;
    struct FlatImage *img = flat_image_create(width, height);
    int x, y;
    for (y=0; y < img->height; y++) {
        for(x=0; x < img->width; x++) {
            img->buffer[y * width + x] = c;
        }
    }
    return img;
}
