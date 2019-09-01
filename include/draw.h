#pragma once
#include <basic.h>

struct RoyalPixel get_pixel(struct image* img, int cor[2]);
struct rgb flat_image_get_pixel(struct FlatImage* img, int cor[2]);
struct rgb flat_image_get_aa_pixel(struct FlatImage* img, float cor[2]);
void set_pixel(struct image* img, int cor[2], struct RoyalPixel color);
void set_aa_pixel(struct image* img, float cor[2], struct RoyalPixel color);
