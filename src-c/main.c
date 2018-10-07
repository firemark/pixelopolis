#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

#define PNG_DEBUG 3
#include <png.h>

struct rgb {uint8_t r, g, b;};

void _png_fill_rows(png_structp png_ptr, int width, int height, struct rgb* buffer) {
    int x, y;
    png_bytep row = malloc(3 * width * sizeof(png_byte));

    for (y=0; y < height; y++) {
        for(x=0; x < width; x++) {
            struct rgb* pixel = &buffer[y * width + x];
            row[x*3 + 0] = pixel->r;
            row[x*3 + 1] = pixel->g;
            row[x*3 + 2] = pixel->b;
        }
        png_write_row(png_ptr, row);
    }

    free(row);
}

int write_png_file(char* file_name, int width, int height, struct rgb* buffer) {
    int code = 0;
    FILE *fp = NULL;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;

    /* create file */
    fp = fopen(file_name, "wb");
    if (!fp) {
        fprintf(stderr, "Problem with creating file %s\n", file_name);
        code = 1;
        goto finalize;
    }

    /* initialize stuff */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fprintf(stderr, "Problem with creating PNG\n");
        code = 1;
        goto finalize;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        fprintf(stderr, "Problem with creating PNG\n");
        code = 1;
        goto finalize;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Problem with creating PNG\n");
        code = 1;
        goto finalize;
    }

    png_init_io(png_ptr, fp);

    /* write header */
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Problem with creating PNG Header\n");
        code = 1;
        goto finalize;
    }

    png_set_IHDR(png_ptr, info_ptr, width, height,
                 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    /* write bytes */
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Problem with writing file\n");
        code = 1;
        goto finalize;
    }

    _png_fill_rows(png_ptr, width, height, buffer);

    /* end write */
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Problem with end writing\n");
        code = 1;
        goto finalize;
    }

    png_write_end(png_ptr, NULL);

    finalize:
    if (fp) fclose(fp);
    if (info_ptr) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    if (png_ptr) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    return code;
}


int main(int argc, char **argv) {
    int w = 800;
    int h = 600;
    struct rgb* buffer = malloc(w * h * sizeof(struct rgb));
    int x, y;
    for (x=0; x < w; x++)
        for (y=0; y < h; y++) {
            struct rgb temp = {.r=x & 0xff, .g=y & 0xff, .b=0};
            buffer[y * w + x] = temp;
        }
    write_png_file("marek.png", w, h, buffer);
    return 0;
}
