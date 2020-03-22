#pragma once
#include "img.h"
#include "draw_builder.h"

#define POINT(v, a, b, c) v[0] + a, v[1] + b, v[2] + c

struct DrawInnerInfo {
    struct image *img; 
    int *vox;
    int *out_vox;
};

enum Valign { VALIGN_TOP, VALIGN_BOTTOM };


// texture
struct FlatImage* css_draw_make_texture_from_wall(struct WallObj *obj, int width, int height);

// base
void css_base_draw_plane(struct image* img, struct FlatImage* img_to_draw, int voxes[12]);
void css_base_draw_plane_with_uv(struct image* img, struct FlatImage* img_to_draw, int voxes[12], int uv[4]);
void css_base_draw_wide_triangle(struct image* img, struct FlatImage* img_to_draw, int voxes[9]);

// draw objs
void css_draw_void(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info);
void css_draw_cube(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info);
void css_draw_cone(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info);
void css_draw_cylinder(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info);
void css_draw_triangle(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info);
void css_draw_pyramid(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info);
void css_draw_series(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info);
