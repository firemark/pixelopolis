#pragma once
#include <stdint.h>

#include "pixelopolis/basic.h"
#include "pixelopolis/memory.h"

struct image* create_black_image(const int width, const int height);
struct FlatImage* flat_image_create(const int width, const int height);
struct FlatImage* flat_image_create_memory(struct Memory* memory, const int width, const int height);
struct FlatImage* flat_image_create_with_color(const int width, const int height, const struct rgb* color);
struct OneChanImage* one_chan_image_create(const int width, const int height);
struct OneChanImage* one_chan_image_create_memory(struct Memory*, const int width, const int height);
struct OneChanImage* one_chan_image_create_with_color(const int width, const int height, uint8_t color);
struct FloatImage* float_image_create(const int width, const int height);
struct FloatImage* float_image_create_memory(struct Memory* memory, const int width, const int height);
struct FloatImage* float_image_create_with_color(const int width, const int height, const struct xyz* color);
