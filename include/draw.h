#pragma once
#include <basic.h>

struct RoyalPixel get_pixel(const struct image* img, const int cor[2]);
struct rgb flat_image_get_pixel(const struct FlatImage* img, const int cor[2]);
struct rgb flat_image_get_aa_pixel(const struct FlatImage* img, const double cor[2]);
void set_pixel(struct image* img, const int cor[2], const struct RoyalPixel color);
void set_aa_pixel(struct image* img, const double cor[2], const struct RoyalPixel color);
