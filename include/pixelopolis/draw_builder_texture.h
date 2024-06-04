#pragma once
#include <stdint.h>

#include "basic.h"
#include "css.h"
#include "draw_builder_common.h"

enum TexPartDirection { TEX_PART_VERTICAL = 0, TEX_PART_HORIZONTAL = 2 };

struct TexDefaultObj {
    struct FlatImage* texture;
    struct FloatImage* normal_map;
    struct rgb* color;
};

struct TexVoidObj {
    struct TexObj* child;
};

struct ShiftTexPair {
    int shift;
    struct TexObj* obj;
};

struct TexPartObj {
    enum TexPartDirection direction;
    struct TexObj* background;
    struct ShiftTexPair** objs;
    uint32_t objs_size;
    uint32_t padding;
    uint32_t length;
};

struct TileInfo {
    struct TexObj* obj;
    uint32_t depth;
    uint32_t width;
};

struct TexTileObj {
    struct TileInfo** infos;
    struct rgb* inner_border_color;
    struct rgb* outer_border_color;
    uint32_t info_size;
    uint32_t height;
    uint32_t shift;
    uint32_t shift_seed;
};

void builder_texture_init(void);
void builder_texture_stop(void);

struct TexObj* builder_texture_make(struct Program* program, struct Rule* parent, struct RuleSelector* query, int width, int height);
