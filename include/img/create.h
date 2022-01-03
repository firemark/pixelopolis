#pragma once
#include "../basic.h"

struct image* create_black_image(const int width, const int height);
struct FlatImage* flat_image_create(const int width, const int height);
struct FlatImage* flat_image_create_with_color(const int width, const int height, const struct rgb* color);
struct FloatImage* float_image_create(const int width, const int height);
