#pragma once
#include <basic.h>
#include <draw.h>


void draw_plane(
        struct image *img, struct image *img_to_draw,
        int vox[3],
        int width, int height,
        enum direction dir);
