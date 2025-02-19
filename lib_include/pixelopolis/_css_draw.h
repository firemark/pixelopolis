#pragma once
#include <stdlib.h>

#include "pixelopolis/draw_builder_world.h"
#include "pixelopolis/img.h"
#include "pixelopolis/img/draw_poly.h"
#include "pixelopolis/texture_draw.h"
#include "pixelopolis/world_draw.h"

struct DrawInnerInfo {
    struct image *img;
    int *vox;
    int *out_vox;
};

enum Valign { VALIGN_TOP, VALIGN_BOTTOM };

// base
struct PolyInfo poly_info_create(struct WallObj *wall, int width, int height,
                                 struct DrawInnerInfo *inner_info);
void css_base_draw_plane(struct PolyInfo *info, const int voxes[12], const struct WallObj *wall);
void css_base_draw_plane_with_uv(struct PolyInfo *info, const int voxes[12], const int uv[4],
                                 const struct WallObj *wall);
void css_base_draw_wide_triangle(struct PolyInfo *info, const int voxes[9],
                                 const struct WallObj *wall);
void css_base_draw_poly_random(struct image *img, const int voxes[9], const struct WallObj *wall);

// draw objs
void css_draw_void(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info);
void css_draw_cube(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info);
void css_draw_cone(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info);
void css_draw_cylinder(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info);
void css_draw_dome(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info);
void css_draw_triangle(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info);
void css_draw_pyramid(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info);
void css_draw_series(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info);
void css_draw_board(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info);

// utils
static inline int _x_rotate(const int x, const int y, const struct BasicObj *basic) {
    return x * basic->cos_th - y * basic->sin_th;
}

static inline int _y_rotate(const int x, const int y, const struct BasicObj *basic) {
    return x * basic->sin_th + y * basic->cos_th;
}

static inline void _transform(int *voxes, const int vox[3], const struct BasicObj *basic,
                              const size_t size) {
    size_t i;
    const size_t total_size = size * 3;
    const int vox_x = vox[0];
    const int vox_y = vox[1];
    const int vox_z = vox[2];
    for (i = 0; i < total_size; i += 3) {
        const int x = voxes[i + 0];
        const int y = voxes[i + 1];
        const int z = voxes[i + 2];
        voxes[i + 0] = vox_x + _x_rotate(x, y, basic);
        voxes[i + 1] = vox_y + _y_rotate(x, y, basic);
        voxes[i + 2] = vox_z + z;
    }
}

static inline void _do_justify(int vox[3], const enum Justify justify, const int x, const int y) {
    switch (justify) {
        case JUSTIFY_START:
            break;  // do nothing
        case JUSTIFY_END:
            vox[0] += x;
            vox[1] += y;
            break;
        case JUSTIFY_CENTER:
            vox[0] += x / 2;
            vox[1] += y / 2;
            break;
        case JUSTIFY_RANDOM:
            if (x > 0) vox[0] += rand() % x;
            if (x < 0) vox[0] -= rand() % -x;
            if (y > 0) vox[1] += rand() % y;
            if (y < 0) vox[1] -= rand() % -y;
            break;
    }
}

static inline void _justify_v(int vox[3], const struct BasicObj *basic,
                              const struct BasicObj *parent_basic) {
    const int sub = parent_basic->width - basic->width;
    if (!sub) return;
    const int sub_x = _x_rotate(sub, 0, parent_basic);
    const int sub_y = _y_rotate(sub, 0, parent_basic);
    _do_justify(vox, parent_basic->v_justify, sub_x, sub_y);
}

static inline void _justify_d(int vox[3], const struct BasicObj *basic,
                              const struct BasicObj *parent_basic) {
    const int sub = parent_basic->depth - basic->depth;
    if (!sub) return;
    const int sub_x = _x_rotate(0, sub, parent_basic);
    const int sub_y = _y_rotate(0, sub, parent_basic);
    _do_justify(vox, parent_basic->d_justify, sub_x, sub_y);
}

static inline void _justify_vd(int vox[3], const struct BasicObj *basic,
                               const struct BasicObj *parent_basic) {
    _justify_v(vox, basic, parent_basic);
    _justify_d(vox, basic, parent_basic);
}
