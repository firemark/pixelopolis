#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <png.h>
#include <limits.h>

#include <img.h>


void _fill_rows_from_image_to_png(struct image* img, png_structp png_ptr) {
    int x, y;
    png_bytep row = malloc(3 * img->width * sizeof(png_byte));

    for (y=0; y < img->height; y++) {
        png_bytep ptr = row;
        for(x=0; x < img->width; x++) {
            struct RoyalPixel* pixel = &img->buffer[y * img->width + x];
            ptr[0] = pixel->r;
            ptr[1] = pixel->g;
            ptr[2] = pixel->b;
            ptr += 3;
        }
        png_write_row(png_ptr, row);
    }

    free(row);
}

void _fill_rows_from_png_to_image(png_structp png_ptr, struct FlatImage* img, int pixel_size) {
    int x, y;
    png_bytep row = malloc(pixel_size * img->width * sizeof(png_byte));

    for (y=0; y < img->height; y++) {
        png_bytep ptr = row;
        png_read_row(png_ptr, ptr, NULL);
        for(x=0; x < img->width; x++) {
            struct rgb pixel = {
                .r=ptr[0],
                .g=ptr[1],
                .b=ptr[2],
            };
            ptr += pixel_size;
            img->buffer[y * img->width + x] = pixel;
        }
    }

    free(row);
}

struct image* create_black_image(int width, int height) {
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

struct FlatImage* flat_image_create(int width, int height) {
    struct FlatImage *img = malloc(sizeof(struct FlatImage));
    img->width = width;
    img->height = height;
    img->buffer = malloc(width * height * sizeof(struct rgb));
    return img;
}

size_t __get_index(struct FlatImage* img, int x, int y) {
    y = img->height - 1 - y;
    return y * img->width + x;
}

void flat_image_fill(struct FlatImage* img, struct FlatImage* filler) {
    int width = img->width;
    int height = img->height;
    int filler_width = filler->width;
    int filler_height = filler->height;
    int x, y;
    for (y=0; y < height; y++) {
        for(x=0; x < width; x++) {
            size_t filler_index = __get_index(filler, x % filler_width, y % filler_height);
            size_t img_index = __get_index(img, x, y);
            img->buffer[img_index] = filler->buffer[filler_index];
        }
    }
}

void flat_image_fill_column(struct FlatImage* img, struct FlatImage* filler, int img_y) {
    int width = img->width;
    int height = img->height;
    int filler_width = filler->width;
    int filler_height = filler->height;

    int limit_height = img_y + filler_height < height ? filler_height : height - img_y; 
    int x, y;
    for (y=0; y < limit_height; y++) {
        for(x=0; x < width; x++) {
            size_t filler_index = __get_index(filler, x % filler_width, y);
            size_t img_index = __get_index(img, x, img_y);
            img->buffer[img_index] = filler->buffer[filler_index];
        }
        img_y++;
    }
}

void flat_image_copy(struct FlatImage* img, struct FlatImage* filler, int img_x, int img_y) {
    int width = img->width;
    int height = img->height;
    int filler_width = filler->width;
    int filler_height = filler->height;

    int limit_width = img_x + filler_width < width ? filler_width : width - img_x; 
    int limit_height = img_y + filler_height < height ? filler_height : height - img_y; 
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

struct FlatImage* flat_image_read_png_file(char* file_name) {
    unsigned char header[8];
    FILE *fp = NULL;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    struct FlatImage *img = NULL;

    /* create file */
    fp = fopen(file_name, "rb");
    if (!fp) {
        fprintf(stderr, "Problem with reading file %s\n", file_name);
        goto finalize;
    }

    size_t header_size = fread(header, 1, 8, fp);
    if (header_size != 8 || png_sig_cmp(header, 0, 8)) {
        fprintf(stderr, "File %s is not a valid PNG\n", file_name);
        goto finalize;
    }

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fprintf(stderr, "Problem with loading PNG\n");
        goto finalize;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        fprintf(stderr, "Problem with loading PNG\n");
        goto finalize;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Problem with loading PNG\n");
        goto finalize;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    int width = png_get_image_width(png_ptr, info_ptr);
    int height = png_get_image_height(png_ptr, info_ptr);
    int pixel_size;

    png_byte color_type = png_get_color_type(png_ptr, info_ptr);
    switch (color_type) {
        case PNG_COLOR_TYPE_RGB: pixel_size = 3; break;
        case PNG_COLOR_TYPE_RGBA: pixel_size = 4; break;
        default: 
        fprintf(stderr, "File %s hasn't RGB/RGBA type:%d \n", file_name, color_type);
        goto finalize;
    }

    png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    if (bit_depth != 8) {
        fprintf(stderr, "File %s hasn't 8bit depth\n", file_name);
        goto finalize;
    }

    //int number_of_passes = png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, info_ptr);

    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Problem with loading %s\n", file_name);
        goto finalize;
    }

    img = malloc(sizeof(struct FlatImage));
    img->width = width;
    img->height = height;
    img->buffer = malloc(width * height * sizeof(struct FlatImage));

    _fill_rows_from_png_to_image(png_ptr, img, pixel_size);

    png_read_end(png_ptr, info_ptr);

    finalize:
    if (fp) fclose(fp);
    if (png_ptr) {
        if (info_ptr) png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        else png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
    }
    
    return img;
}


int write_png_file(FILE *fp, struct image* img) {
    int code = 0;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;

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

    png_set_IHDR(png_ptr, info_ptr, img->width, img->height,
                 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    /* write bytes */
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Problem with writing file\n");
        code = 1;
        goto finalize;
    }
    _fill_rows_from_image_to_png(img, png_ptr);

    /* end write */
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

void destroy_image(struct image *img) {
    free(img->buffer);
    free(img);
}

void destroy_flat_image(struct FlatImage *img) {
    free(img->buffer);
    free(img);
}
