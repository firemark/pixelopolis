#pragma once
#include "pixelopolis/basic.h"

struct RoyalPixel get_pixel(const struct image* img, const int cor[2]);
struct RoyalPixel* get_pixel_ptr(const struct image* img, const int cor[2]);
struct rgb flat_image_get_pixel(const struct FlatImage* img, const int cor[2]);
struct rgb* flat_image_get_pixel_ptr(const struct FlatImage* img, const int cor[2]);
uint8_t one_chan_image_get_pixel(const struct OneChanImage* img, const int cor[2]);
struct xyz float_image_get_pixel(const struct FloatImage* img, const int cor[2]);
struct rgb flat_image_get_aa_pixel(const struct FlatImage* img, const double cor[2]);

void set_pixel(struct image* img, const int cor[2], const struct RoyalPixel color);
void flat_image_set_pixel(const struct FlatImage* img, const int cor[2], const struct rgb pixel);
void float_image_set_pixel(const struct FloatImage* img, const int cor[2], const struct xyz pixel);
void one_chan_image_set_pixel(const struct OneChanImage* img, const int cor[2],
                              const unsigned char pixel);
void set_aa_pixel(struct image* img, const double cor[2], const struct RoyalPixel color);
