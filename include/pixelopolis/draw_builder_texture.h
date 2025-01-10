#pragma once
#include <stdint.h>

#include "basic.h"
#include "css.h"
#include "draw_builder_common.h"

enum TexPartDirection { TEX_PART_VERTICAL = 'V', TEX_PART_HORIZONTAL = 'H' };

struct TexDefaultObj {
    struct FlatImage* texture;
    struct FloatImage* normal_map;
    struct rgb* color;
};

struct TexVoidObj {
    struct TexObj* child;
};

struct ShiftTexPair {
    int shift[2];
    struct TexObj* obj;
};

struct TexPartObj {
    struct TexObj* background;
    struct ShiftTexPair** objs;
};

void builder_texture_init(void);
void builder_texture_stop(void);

struct TexObj* builder_texture_make(struct Program* program, struct RuleWithParent* parent,
                                    struct RuleSelector* query, int width, int height);
