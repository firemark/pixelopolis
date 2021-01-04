#pragma once
#include "css.h"
#include "img.h"
#include "draw_builder.h"

enum TexFilterAlgorithm {
    FILTER_ALGORITHM_NONE,
    FILTER_ALGORITHM_SCALE2,
    FILTER_ALGORITHM_SCALE3,
    FILTER_ALGORITHM_MAME2,
    FILTER_ALGORITHM_MAME4,
};

struct DrawInfo {
    struct image *img;
    int *vox;
    enum TexFilterAlgorithm filter;
};


void draw_component(struct DrawObj* draw_obj, struct DrawInfo *info, int *out_vox);
