#pragma once
#include <stdio.h>

#include "basic.h"

struct image* create_black_image(int width, int height);
struct image* read_png_file(char* filename);
void destroy_image(struct image *img);

int write_png_file(FILE *fp, struct image* img);
