#pragma once
#include "css.h"
#include "img.h"

struct DrawInfo {
    struct image *img;
    struct Program *program;
    struct RuleSelector *query;
    int *vox;
};

void draw_component(struct DrawInfo *info, int *out_vox);
