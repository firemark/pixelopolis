#pragma once
#include <stdio.h>

#include "basic.h"

struct image* create_black_image(const int width, const int height);
void destroy_image(struct image *img);

struct FlatImage* flat_image_create(const int width, const int height);
struct FlatImage* flat_image_create_with_color(const int width, const int height, const struct rgb* color);
struct FlatImage* flat_image_read_png_file(const char* filename);
void flat_image_fill(struct FlatImage* img, const struct FlatImage* filler);
void flat_image_fill_column(struct FlatImage* img, const struct FlatImage* filler, int img_y);
void flat_image_copy(struct FlatImage* img, const struct FlatImage* filler, int img_x, int img_y);
void flat_image_destroy(struct FlatImage *img);

int write_png_file(FILE *fp, const struct image* img);
