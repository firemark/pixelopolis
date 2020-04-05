#pragma once
#include "img.h"
#include "draw_builder.h"

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
void css_draw_dome(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info);
void css_draw_triangle(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info);
void css_draw_pyramid(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info);
void css_draw_series(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info);
void css_draw_square_fence(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info);

// utils
static inline const int _x_rotate(
        const int x, const int y,
        const struct BasicObj* basic) {
    return x * basic->cos_th - y * basic->sin_th;
}

static inline const int _y_rotate(
        const int x, const int y,
        const struct BasicObj* basic) {
    return x * basic->sin_th + y * basic->cos_th;
}

static inline void _transform(int *voxes, const int vox[3], const struct BasicObj* basic, const size_t size) {
    size_t i;
    const size_t total_size = size * 3;
    const int vox_x = vox[0];
    const int vox_y = vox[1];
    const int vox_z = vox[2];
    for(i = 0; i < total_size ; i += 3) {
        const int x = voxes[i + 0];
        const int y = voxes[i + 1];
        const int z = voxes[i + 2];
        voxes[i + 0] = vox_x + _x_rotate(x, y, basic);
        voxes[i + 1] = vox_y + _y_rotate(x, y, basic);
        voxes[i + 2] = vox_z + z;
    }
}
