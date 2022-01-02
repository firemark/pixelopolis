#pragma once
#include "../basic.h"

struct FlatImage* flat_image_read_png_file(const char* filename);
struct OneChanImage* one_chan_image_read_png_file(const char* filename);
