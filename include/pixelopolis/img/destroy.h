#pragma once
#include "../basic.h"

void destroy_image(struct image* img);
void flat_image_destroy(struct FlatImage* img);
void one_chan_image_destroy(struct OneChanImage* img);
void float_image_destroy(struct FloatImage* img);
