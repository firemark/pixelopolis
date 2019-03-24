#pragma once
#include "css.h"
#include "img.h"
#include "draw_builder.h"

struct DrawInfo {
    struct image *img;
    int *vox;
};

void draw_component(struct DrawObj* draw_obj, struct DrawInfo *info, int *out_vox);
