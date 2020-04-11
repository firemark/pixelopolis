#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "_css_draw.h"
#include "angle_iter.h"
#include "draw_poly.h"


static void _css_draw_cone_roof(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    const struct ConeObj *obj = draw_obj->obj;
    const int width = draw_obj->basic.width;
    const int height = draw_obj->basic.height;
    const int depth = draw_obj->basic.depth;
    const int *vox = inner_info->vox;

    struct WallObj *roof = obj->roof;
    const int wh = width / 2;
    const int dh = depth / 2;

    struct FlatImage* img_to_draw = css_draw_make_texture_from_wall(roof, roof->width, roof->height);

    const int total_length = roof->width;
    int iter_length = 0;

    struct AngleIter angle_iter;
    angle_iter_start(&angle_iter, width, depth, obj->sides);
    while(angle_iter_iterate(&angle_iter)) {
        int voxes[9] = {
            angle_iter.x  + wh, angle_iter.y  + dh, 0,
            angle_iter.nx + wh, angle_iter.ny + dh, 0,
            wh, dh, height,
        };
        _transform(voxes, vox, &draw_obj->basic, 3);

        int next_length = iter_length + angle_iter_get_length(&angle_iter);
        if (next_length > total_length) next_length = total_length;

        int uv[6] = {
            iter_length, 0,
            next_length, 0,
            (iter_length + next_length) / 2, roof->height,
        };

        draw_poly(inner_info->img, img_to_draw, voxes, uv);
        css_base_draw_poly_random(inner_info->img, voxes, roof);

        iter_length = next_length;
    }
    free(img_to_draw);
}


void css_draw_cone(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    struct ConeObj *obj = draw_obj->obj;
    if (!obj) return;

    if (obj->roof) {
        _css_draw_cone_roof(draw_obj, inner_info);
    }

    int *out_vox = inner_info->out_vox;
    if (out_vox) {
        out_vox[0] = draw_obj->basic.width;
        out_vox[1] = draw_obj->basic.depth;
        out_vox[2] = draw_obj->basic.height;
    }
}
