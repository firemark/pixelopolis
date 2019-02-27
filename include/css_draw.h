#pragma once
#include "css.h"
#include "img.h"

void css_draw_wall(
        struct image *img, 
        struct Program *program,
        struct RuleSelector *query,
        int vox[3],
        enum direction dir);
