#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "pixelopolis/_css_draw.h"
#include "pixelopolis/angle_iter.h"
#include "pixelopolis/img/draw_poly.h"

#define NORMALIZE_VAL 200

static void _css_draw_dome_roof(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    const struct DomeObj *obj = draw_obj->obj;
    const int width = draw_obj->basic.width;
    const int height = draw_obj->basic.height;
    const int depth = draw_obj->basic.depth;

    struct WallObj *roof = obj->roof;
    const int wh = width / 2;
    const int dh = depth / 2;
    const int h2 = height * 2;
    struct PolyInfo poly_info = poly_info_create(roof, roof->width, roof->height, inner_info);

    int w_length = 0;
    struct AngleIter angle_iter;
    angle_iter_start(&angle_iter, width, depth, obj->vertical_sides);
    while (angle_iter_iterate(&angle_iter)) {
        const int nw_length = w_length + angle_iter_get_length(&angle_iter);

        int h_length = 0;
        struct AngleIter height_angle_iter;
        angle_iter_start_with_range(&height_angle_iter, NORMALIZE_VAL, h2,
                                    obj->horizontal_sides * 4, 0, obj->horizontal_sides - 1);
        while (angle_iter_iterate(&height_angle_iter)) {
#define NORM(val, n) (val * 2 * height_angle_iter.n / NORMALIZE_VAL)
            // s - start
            // e - end
            // n - next
            const int sx = NORM(angle_iter.x, x);
            const int sy = NORM(angle_iter.y, x);
            const int snx = NORM(angle_iter.nx, x);
            const int sny = NORM(angle_iter.ny, x);
            const int ex = NORM(angle_iter.x, nx);
            const int ey = NORM(angle_iter.y, nx);
            const int enx = NORM(angle_iter.nx, nx);
            const int eny = NORM(angle_iter.ny, nx);
#undef NORM
            int voxes[12] = {
                sx + wh, sy + dh, height_angle_iter.y,  snx + wh, sny + dh, height_angle_iter.y,
                ex + wh, ey + dh, height_angle_iter.ny, enx + wh, eny + dh, height_angle_iter.ny,
            };
            _transform(voxes, inner_info->vox, &draw_obj->basic, 4);

            const int nh_length = h_length + angle_iter_get_length(&height_angle_iter);

            int uv[4] = {
                w_length, h_length,    //
                nw_length, nh_length,  //
            };

            css_base_draw_plane_with_uv(&poly_info, voxes, uv, roof);
            h_length = nh_length;
        }
        w_length = nw_length;
    }
    poly_info_clear(&poly_info);
}

void css_draw_dome(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    struct ConeObj *obj = draw_obj->obj;
    if (!obj) return;

    if (obj->roof) {
        _css_draw_dome_roof(draw_obj, inner_info);
    }

    int *out_vox = inner_info->out_vox;
    if (out_vox) {
        out_vox[0] = draw_obj->basic.width;
        out_vox[1] = draw_obj->basic.depth;
        out_vox[2] = draw_obj->basic.height;
    }
}
