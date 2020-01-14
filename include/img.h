#pragma once
#include <stdio.h>

#include "basic.h"

struct image* create_black_image(int width, int height);
void destroy_image(struct image *img);

struct FlatImage* flat_image_create(int width, int height);
struct FlatImage* flat_image_create_with_color(int width, int height, struct rgb* color);
struct FlatImage* flat_image_read_png_file(char* filename);
void flat_image_fill(struct FlatImage* img, struct FlatImage* filler);
void flat_image_fill_column(struct FlatImage* img, struct FlatImage* filler, int img_y);
void flat_image_copy(struct FlatImage* img, struct FlatImage* filler, int img_x, int img_y);
void flat_image_destroy(struct FlatImage *img);

int write_png_file(FILE *fp, struct image* img);
