#pragma once
#include "pixelopolis/basic.h"

struct FloatImage* transform_bump_to_normal_map(struct OneChanImage* bump_map);
struct FlatImage* tranform_normal_map_to_rgb_map(struct FloatImage* normal_map);
