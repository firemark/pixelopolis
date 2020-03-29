#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "_css_draw.h"
#include "angle_iter.h"
#include "draw_poly.h"

#define NORMALIZE_VAL 200


void _css_draw_dome_roof(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    struct DomeObj *obj = draw_obj->obj;
    int width = draw_obj->basic.width;
    int height = draw_obj->basic.height;
    int depth = draw_obj->basic.depth;
    int *vox = inner_info->vox;

    struct WallObj *roof = obj->roof;
    int wh = width / 2;
    int dh = depth / 2;

    struct FlatImage* img_to_draw = css_draw_make_texture_from_wall(roof, roof->width, roof->height);

    int w_length = 0;
    struct AngleIter angle_iter;
    angle_iter_start(&angle_iter, width, depth, obj->vertical_sides);
    while(angle_iter_iterate(&angle_iter)) {
        int nw_length = w_length + angle_iter_get_length(&angle_iter);

        int h_length = 0;
        struct AngleIter height_angle_iter;
        angle_iter_start_with_range(&height_angle_iter, NORMALIZE_VAL, height, obj->horizontal_sides * 4, 0, obj->horizontal_sides - 1);
        while(angle_iter_iterate(&height_angle_iter)) {
#define NORM(val, n) (val * 2 * height_angle_iter . n / NORMALIZE_VAL)
            // s - start
            // e - end
            // n - next
            int  sx = NORM(angle_iter.x , x);
            int  sy = NORM(angle_iter.y , x);
            int snx = NORM(angle_iter.nx, x);
            int sny = NORM(angle_iter.ny, x);
            int  ex = NORM(angle_iter.x , nx);
            int  ey = NORM(angle_iter.y , nx);
            int enx = NORM(angle_iter.nx, nx);
            int eny = NORM(angle_iter.ny, nx);
#undef NORM
            int voxes[12] = {
                POINT(vox,  sx + wh,  sy + dh, height_angle_iter.y),
                POINT(vox, snx + wh, sny + dh, height_angle_iter.y),
                POINT(vox,  ex + wh,  ey + dh, height_angle_iter.ny),
                POINT(vox, enx + wh, eny + dh, height_angle_iter.ny),
            };

            int nh_length = h_length + angle_iter_get_length(&height_angle_iter);

            int uv[4] = {
                 w_length,  h_length,
                nw_length, nh_length,
            };

            css_base_draw_plane_with_uv(inner_info->img, img_to_draw, voxes, uv);
            h_length = nh_length;
        }
        w_length = nw_length;
    }
    free(img_to_draw);
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
