#pragma once
#include "../basic.h"

void flat_image_fill(struct FlatImage* img, const struct FlatImage* filler);
void flat_image_fill_column(struct FlatImage* img, const struct FlatImage* filler, int img_y);
void flat_image_copy(struct FlatImage* img, const struct FlatImage* filler, int img_x, int img_y);
