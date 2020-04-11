#pragma once
#include <basic.h>

void draw_poly(
        struct image *img,
        struct FlatImage *img_to_draw,
        int voxes[9],
        int uv[6]);

void draw_sprite(
        struct image *img,
        const struct FlatImage *img_to_draw,
        const int vox[3],
        const double normal[3]);

void draw_sprites_in_random_position_in_poly(
        struct image *img,
        const struct FlatImage *img_to_draw,
        const int voxes[3],
        const int density);
