#pragma once
#include <basic.h>

struct rgb get_pixel(struct image* img, int cor[2]);
void set_pixel(struct image* img, int cor[2], struct rgb color);
void set_aa_pixel(struct image* img, float cor[2], struct rgb color);
