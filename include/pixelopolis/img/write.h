#pragma once
#include <stdio.h>

#include "../basic.h"

int write_png_file_from_image(FILE* fp, const struct image* img);
int write_png_file_from_flat_image(FILE* fp, const struct FlatImage* img);
