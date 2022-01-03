#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "_css_draw.h"
#include "angle_iter.h"
#include "draw_poly.h"

#define CREATE_VOXES(angle_iter, basic) { \
    basic.width / 2 + angle_iter.x , basic.depth / 2 + angle_iter.y , 0, \
    basic.width / 2 + angle_iter.nx, basic.depth / 2 + angle_iter.ny, 0, \
    basic.width / 2 + angle_iter.x , basic.depth / 2 + angle_iter.y , basic.height, \
    basic.width / 2 + angle_iter.nx, basic.depth / 2 + angle_iter.ny, basic.height, \
}


static void _css_draw_cylinder_many_walls(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    const struct CylinderObj *obj = draw_obj->obj;
    const int width = draw_obj->basic.width;
    const int height = draw_obj->basic.height;
    const int depth = draw_obj->basic.depth;

    struct WallObj **walls = obj->walls;

    struct AngleIter angle_iter;
    angle_iter_start(&angle_iter, width, depth, obj->sides);
    while(angle_iter_iterate(&angle_iter)) {
        int voxes[12] = CREATE_VOXES(angle_iter, draw_obj->basic);
        _transform(voxes, inner_info->vox, &draw_obj->basic, 4);
        struct WallObj *wall = walls[angle_iter.i];

        struct DrawTexInfo tex_info = {
            .wall=wall,
            .size={wall->width, height},
            .filter=inner_info->filter,
        };
        struct PolyInfo poly_info = poly_info_create(&tex_info, inner_info);
        css_base_draw_plane(&poly_info, voxes, wall);
        poly_info_clear(&poly_info);
    }
}

static void _css_draw_cylinder_single_wall(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    const struct CylinderObj *obj = draw_obj->obj;
    const int width = draw_obj->basic.width;
    const int height = draw_obj->basic.height;
    const int depth = draw_obj->basic.depth;

    struct WallObj *wall = obj->walls[0];

    int total_length = wall->width;
    int iter_length = 0;

    struct DrawTexInfo tex_info = {
        .wall=wall,
        .size={total_length, height},
        .filter=inner_info->filter,
    };
    struct PolyInfo poly_info = poly_info_create(&tex_info, inner_info);

    struct AngleIter angle_iter;
    angle_iter_start(&angle_iter, width, depth, obj->sides);
    while(angle_iter_iterate(&angle_iter)) {
        int voxes[12] = CREATE_VOXES(angle_iter, draw_obj->basic);
        _transform(voxes, inner_info->vox, &draw_obj->basic, 4);

        int next_length = iter_length + angle_iter_get_length(&angle_iter);
        if (next_length > total_length) next_length = total_length;

        int uv[4] = {
            iter_length, 0,
            next_length, height,
        };

        css_base_draw_plane_with_uv(&poly_info, voxes, uv, wall);
        iter_length = next_length;
    }

    poly_info_clear(&poly_info);
}

static void _css_draw_cylinder_roof(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    const struct CylinderObj *obj = draw_obj->obj;
    const int width = draw_obj->basic.width;
    const int height = draw_obj->basic.height;
    const int depth = draw_obj->basic.depth;
    const int *vox = inner_info->vox;

    struct WallObj *roof = obj->roof;
    const int wh = width / 2;
    const int dh = depth / 2;

    struct DrawTexInfo tex_info = {
        .wall=roof,
        .size={roof->width, roof->height},
        .filter=inner_info->filter,
    };
    struct PolyInfo poly_info = poly_info_create(&tex_info, inner_info);

    struct AngleIter angle_iter;
    angle_iter_start(&angle_iter, width, depth, obj->sides);
    while(angle_iter_iterate(&angle_iter)) {
        int voxes[9] = {
            angle_iter.x + wh, angle_iter.y + dh, height,
            angle_iter.nx + wh, angle_iter.ny + dh, height,
            wh, dh, height,
        };
        _transform(voxes, vox, &draw_obj->basic, 3);

        int uv[6] = {
            angle_iter.x + wh, angle_iter.y + dh,
            angle_iter.nx + wh, angle_iter.ny + dh,
            wh, dh,
        };

        draw_poly(&poly_info, voxes, uv);
        css_base_draw_poly_random(inner_info->img, voxes, roof);
    }
    poly_info_clear(&poly_info);
}


void css_draw_cylinder(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    struct CylinderObj *obj = draw_obj->obj;
    if (!obj) return;

    if (obj->walls) {
        if (obj->has_many_walls) {
            _css_draw_cylinder_many_walls(draw_obj, inner_info);
        } else {
            _css_draw_cylinder_single_wall(draw_obj, inner_info);
        }
    }

    if (obj->roof) {
        _css_draw_cylinder_roof(draw_obj, inner_info);
    }

    int *out_vox = inner_info->out_vox;
    if (out_vox) {
        out_vox[0] = draw_obj->basic.width;
        out_vox[1] = draw_obj->basic.depth;
        out_vox[2] = draw_obj->basic.height;
    }
}
