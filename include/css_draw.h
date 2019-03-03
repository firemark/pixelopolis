#pragma once
#include "css.h"
#include "img.h"

struct DrawInfo {
    struct image *img;
    struct Program *program;
    struct RuleSelector *query;
    int *vox;
};

void css_draw_wall(
        struct DrawInfo* draw_info,
        enum direction dir);

void css_draw_cube(struct DrawInfo* draw_info);
