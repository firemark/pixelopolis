#pragma once
#include "pixelopolis/basic.h"

struct PolyInfo {
    struct image *img;
    struct FlatImage *img_to_draw;
    struct FloatImage *normal_map;
};

void draw_poly(struct PolyInfo *info, const int voxes[9], const int uv[6]);

void draw_sprite(struct image *img, struct FlatImage *img_to_draw, const int vox[3],
                 const double normal[3]);

void draw_sprites_in_random_position_in_poly(struct image *img, struct FlatImage *img_to_draw,
                                             const int voxes[9], const int density);

void poly_info_clear(struct PolyInfo *info);
