#pragma once
#include <basic.h>

struct image* create_black_image(int width, int height);
int write_png_file(char* file_name, struct image* img);
