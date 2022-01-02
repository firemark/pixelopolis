#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <png.h>
#include <limits.h>

#include <img/ops.h>

static inline size_t __get_index(const struct FlatImage* img, const int x, const int y) {
    const int real_y = img->height - 1 - y;
    return real_y * img->width + x;
}

void flat_image_fill(struct FlatImage* img, const struct FlatImage* filler) {
    const int width = img->width;
    const int height = img->height;
    const int filler_width = filler->width;
    const int filler_height = filler->height;
    int x, y;
    for (y=0; y < height; y++) {
        for(x=0; x < width; x++) {
            const size_t filler_index = __get_index(filler, x % filler_width, y % filler_height);
            const size_t img_index = __get_index(img, x, y);
            img->buffer[img_index] = filler->buffer[filler_index];
        }
    }
}

void flat_image_fill_column(struct FlatImage* img, const struct FlatImage* filler, int img_y) {
    const int width = img->width;
    const int height = img->height;
    const int filler_width = filler->width;
    const int filler_height = filler->height;

    const int limit_height = img_y + filler_height < height ? filler_height : height - img_y;
    int x, y;
    for (y=0; y < limit_height; y++) {
        for(x=0; x < width; x++) {
            const size_t filler_index = __get_index(filler, x % filler_width, y);
            const size_t img_index = __get_index(img, x, img_y);
            img->buffer[img_index] = filler->buffer[filler_index];
        }
        img_y++;
    }
}

void flat_image_copy(struct FlatImage* img, const struct FlatImage* filler, int img_x, int img_y) {
    const int width = img->width;
    const int height = img->height;
    const int filler_width = filler->width;
    const int filler_height = filler->height;

    const int limit_width = img_x + filler_width < width ? filler_width : width - img_x;
    const int limit_height = img_y + filler_height < height ? filler_height : height - img_y;
    int x, y, tmp_img_x;
    for (y=0; y < limit_height; y++) {
        tmp_img_x = img_x;
        for(x=0; x < limit_width; x++) {
            size_t filler_index = __get_index(filler, x, y);
            size_t img_index = __get_index(img, tmp_img_x, img_y);
            img->buffer[img_index] = filler->buffer[filler_index];
            tmp_img_x++;
        }
        img_y++;
    }
}
