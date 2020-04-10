#pragma once
#include <basic.h>

void draw_poly(
        struct image *img,
        struct FlatImage *img_to_draw,
        int voxes[9],
        int uv[6]);

void draw_sprite(
        struct image *img,
        struct FlatImage *img_to_draw,
        int vox[3],
        double normal[3]);

void draw_sprite_in_random_position_in_poly(
        struct image *img,
        struct FlatImage *img_to_draw,
        int voxes[9]);
