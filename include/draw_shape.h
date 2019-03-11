#pragma once
#include <basic.h>
#include <draw.h>

struct DrawArgs {
    struct image *img;
    struct image *img_to_draw;
    int *vox;
    int width, height;
    int start_height;
    int max_height;
    enum direction dir;
};

void draw_plane(struct DrawArgs*);
void draw_triangle(struct DrawArgs*);
void draw_plane_with_height(struct DrawArgs*, int zheight);
