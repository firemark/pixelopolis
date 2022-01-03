#include <stdlib.h>
#include "_css_draw.h"

void css_draw_pyramid(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    struct PyramidObj *obj = draw_obj->obj;
    if (!obj) return;
    int *vox = inner_info->vox;
    const int width = draw_obj->basic.width;
    const int height = draw_obj->basic.height;
    const int depth = draw_obj->basic.depth;
    const int w = width;
    const int wh = width / 2;
    const int dh = depth / 2;
    const int h = height;
    const int d = depth;

    struct WallObj *south_wall_obj = obj->south_wall;
    if (south_wall_obj && width > 0 && height > 0) {
        struct DrawTexInfo tex_info = {
            .wall=south_wall_obj,
            .size={south_wall_obj->width, south_wall_obj->height},
            .filter=inner_info->filter,
        };
        struct PolyInfo poly_info_first = {
            .img=inner_info->img,
            .img_to_draw=css_draw_tex(&tex_info),
            .normal_map=NULL,
        };
        struct PolyInfo poly_info_second = {
            .img=inner_info->img,
            .img_to_draw=css_draw_tex(&tex_info),
            .normal_map=NULL,
        };

        int voxes_first[9] = {
            0 , 0 , 0,
            w , 0 , 0,
            wh, dh, h,
        };
        int voxes_second[9] = {
            0 , d , 0,
            w , d , 0,
            wh, dh, h,
        };

        _transform(voxes_first, vox, &draw_obj->basic, 3);
        _transform(voxes_second, vox, &draw_obj->basic, 3);

        css_base_draw_wide_triangle(&poly_info_first, voxes_first, south_wall_obj);
        css_base_draw_wide_triangle(&poly_info_second, voxes_second, south_wall_obj);

        poly_info_clear(&poly_info_first);
        poly_info_clear(&poly_info_second);
    }

    struct WallObj *east_wall_obj = obj->east_wall;
    if (east_wall_obj && width > 0 && height > 0) {
        struct DrawTexInfo tex_info = {
            .wall=east_wall_obj,
            .size={east_wall_obj->width, east_wall_obj->height},
            .filter=inner_info->filter,
        };
        struct PolyInfo poly_info_first = {
            .img=inner_info->img,
            .img_to_draw=css_draw_tex(&tex_info),
            .normal_map=NULL,
        };
        struct PolyInfo poly_info_second = {
            .img=inner_info->img,
            .img_to_draw=css_draw_tex(&tex_info),
            .normal_map=NULL,
        };

        int voxes_first[9] = {
            0 , 0 , 0,
            0 , d , 0,
            wh, dh, h,
        };
        int voxes_second[9] = {
            w , 0 , 0,
            w , d , 0,
            wh, dh, h,
        };

        _transform(voxes_first, vox, &draw_obj->basic, 3);
        _transform(voxes_second, vox, &draw_obj->basic, 3);

        css_base_draw_wide_triangle(&poly_info_first, voxes_second, east_wall_obj);
        css_base_draw_wide_triangle(&poly_info_second, voxes_first, east_wall_obj);

        poly_info_clear(&poly_info_first);
        poly_info_clear(&poly_info_second);
    }

    int *out_vox = inner_info->out_vox;
    if (out_vox) {
        out_vox[0] = width;
        out_vox[1] = depth;
        out_vox[2] = height;
    }
}
