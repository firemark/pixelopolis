#include "pixelopolis/img/read.h"

#include <unistd.h>

#if HAS_PNG
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include <png.h>

struct PngReadMeta {
    int width;
    int height;
    int pixel_size;
};

#define _FROM_PNG(png_ptr, img, meta, type, ...)                                    \
    {                                                                               \
        int x, y;                                                                   \
        img = malloc(sizeof(IMG_TYPE_##type));                                      \
        img->width = meta.width;                                                    \
        img->height = meta.height;                                                  \
        img->buffer = malloc(meta.width * meta.height * sizeof(PIXEL_TYPE_##type)); \
        png_bytep row = malloc(meta.pixel_size * meta.width * sizeof(png_byte));    \
        for (y = 0; y < meta.height; y++) {                                         \
            png_bytep png_pixel = row;                                              \
            png_read_row(png_ptr, png_pixel, NULL);                                 \
            for (x = 0; x < meta.width; x++) {                                      \
                const PIXEL_TYPE_##type pixel = __VA_ARGS__;                        \
                png_pixel += meta.pixel_size;                                       \
                img->buffer[y * meta.width + x] = pixel;                            \
            }                                                                       \
        }                                                                           \
        free(row);                                                                  \
    }

static int _read_png_init(const char* file_name, FILE** fp, png_structp* png_ptr_ptr,
                          png_infop* info_ptr_ptr, struct PngReadMeta* meta) {
    unsigned char header[8];

    *png_ptr_ptr = NULL;
    *info_ptr_ptr = NULL;

    *fp = fopen(file_name, "rb");
    if (!*fp) {
        fprintf(stderr, "Problem with reading file %s\n", file_name);
        return 1;
    }

    size_t header_size = fread(header, 1, 8, *fp);
    if (header_size != 8 || png_sig_cmp(header, 0, 8)) {
        fprintf(stderr, "File %s is not a valid PNG\n", file_name);
        return 1;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    *png_ptr_ptr = png_ptr;
    if (!png_ptr) {
        fprintf(stderr, "Problem with loading PNG\n");
        return 1;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    *info_ptr_ptr = info_ptr;
    if (!info_ptr) {
        fprintf(stderr, "Problem with loading PNG\n");
        return 1;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Problem with loading PNG\n");
        return 1;
    }

    png_init_io(png_ptr, *fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    meta->width = png_get_image_width(png_ptr, info_ptr);
    meta->height = png_get_image_height(png_ptr, info_ptr);

    png_byte color_type = png_get_color_type(png_ptr, info_ptr);
    switch (color_type) {
        case PNG_COLOR_TYPE_RGB:
            meta->pixel_size = 3;
            break;
        case PNG_COLOR_TYPE_RGBA:
            meta->pixel_size = 4;
            break;
        default:
            fprintf(stderr, "File %s hasn't RGB/RGBA type:%d \n", file_name, color_type);
            return 1;
            break;
    }

    png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    if (bit_depth != 8) {
        fprintf(stderr, "File %s hasn't 8bit depth\n", file_name);
        return 1;
    }

    // int number_of_passes = png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, info_ptr);

    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Problem with loading %s\n", file_name);
        return 1;
    }

    return 0;
}

static void _read_png_end(FILE* fp, png_structp png_ptr, png_infop info_ptr) {
    png_read_end(png_ptr, info_ptr);

    if (fp) fclose(fp);
    if (png_ptr) {
        if (info_ptr)
            png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        else
            png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
    }
}

#define _READ_PNG(filename, type, ...)                                        \
    {                                                                         \
        png_structp png_ptr;                                                  \
        png_infop info_ptr;                                                   \
        struct PngReadMeta meta;                                              \
        FILE* fp = NULL;                                                      \
        IMG_TYPE_##type* img = NULL;                                          \
        int code = _read_png_init(filename, &fp, &png_ptr, &info_ptr, &meta); \
        if (code == 0) {                                                      \
            _FROM_PNG(png_ptr, img, meta, type, __VA_ARGS__);                 \
        }                                                                     \
        _read_png_end(fp, png_ptr, info_ptr);                                 \
        return img;                                                           \
    }

struct FlatImage* flat_image_read_png_file(const char* filename) {
    _READ_PNG(filename, FLAT_IMAGE,
              {
                  .r = png_pixel[0],
                  .g = png_pixel[1],
                  .b = png_pixel[2],
              });
}

struct OneChanImage* one_chan_image_read_png_file(const char* filename) {
    _READ_PNG(filename, ONE_CHAN_IMAGE, png_pixel[0]);
}

#else
struct FlatImage* flat_image_read_png_file(const char* filename) { return NULL;}
struct OneChanImage* one_chan_image_read_png_file(const char* filename) { return NULL;}
#endif