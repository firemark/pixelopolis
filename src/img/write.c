#include "pixelopolis/img/write.h"
#include <limits.h>
#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define _TO_PNG(img, png_ptr, type)                                            \
    int x, y;                                                                  \
    png_bytep row = malloc(3 * img->width * sizeof(png_byte));                 \
    for (y = 0; y < img->height; y++) {                                        \
        png_bytep ptr = row;                                                   \
        for (x = 0; x < img->width; x++) {                                     \
            const PIXEL_TYPE_##type* pixel = &img->buffer[y * img->width + x]; \
            ptr[0] = pixel->r;                                                 \
            ptr[1] = pixel->g;                                                 \
            ptr[2] = pixel->b;                                                 \
            ptr += 3;                                                          \
        }                                                                      \
        png_write_row(png_ptr, row);                                           \
    }                                                                          \
    free(row)

static int _png_write_init(FILE* fp, png_structp* png_ptr_ptr, png_infop* info_ptr_ptr, int width,
                           int height) {
    *png_ptr_ptr = NULL;
    *info_ptr_ptr = NULL;

    /* initialize stuff */
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    *png_ptr_ptr = png_ptr;
    if (!png_ptr) {
        fprintf(stderr, "Problem with creating PNG\n");
        return 1;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    *info_ptr_ptr = info_ptr;
    if (!info_ptr) {
        fprintf(stderr, "Problem with creating PNG\n");
        return 1;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Problem with creating PNG\n");
        return 1;
    }
    png_init_io(png_ptr, fp);

    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Problem with creating PNG Header\n");
        return 1;
    }
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Problem with writing file\n");
        return 1;
    }
    return 0;
}

static int _png_write_end(png_structp png_ptr, png_infop info_ptr) {
    static int code;
    code = 0;
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Problem with end writing\n");
        code = 1;
        goto finalize;
    }
    png_write_end(png_ptr, NULL);

finalize:
    if (info_ptr) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    if (png_ptr) png_destroy_write_struct(&png_ptr, &info_ptr);
    return code;
}

#define _WRITE_PNG_FILE(fp, img, type)                                            \
    png_structp png_ptr;                                                          \
    png_infop info_ptr;                                                           \
    int code = _png_write_init(fp, &png_ptr, &info_ptr, img->width, img->height); \
    if (code == 0) {                                                              \
        _TO_PNG(img, png_ptr, type);                                              \
    }                                                                             \
    return _png_write_end(png_ptr, info_ptr) || code;

int write_png_file_from_image(FILE* fp, const struct image* img) {
    _WRITE_PNG_FILE(fp, img, IMAGE);
}

int write_png_file_from_flat_image(FILE* fp, const struct FlatImage* img) {
    _WRITE_PNG_FILE(fp, img, FLAT_IMAGE);
}