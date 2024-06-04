#pragma once
#include <stdbool.h>

#include "draw_builder_common.h"

struct DrawTextureOutput {
    struct FlatImage* texture;
    struct FloatImage* normal_map;
};

struct DrawTexInfo {
    struct DrawTextureOutput* output;
};

void css_texture_draw_component(struct TexObj *tex_obj, struct DrawTexInfo* info);