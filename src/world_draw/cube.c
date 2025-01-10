#include <stdlib.h>

#include "pixelopolis/_css_draw.h"

void css_draw_cube(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    const struct CubeObj *obj = draw_obj->obj;
    if (!obj) return;
    const int *vox = inner_info->vox;
    const int width = draw_obj->basic.width;
    const int height = draw_obj->basic.height;
    const int depth = draw_obj->basic.depth;
    const int w = width;
    const int h = height;
    const int d = depth;

    struct WallObj *east_wall_obj = obj->east_wall;
    if (east_wall_obj && depth > 0 && height > 0) {
        int voxes_first[12] = {
            w, 0, 0,  //
            w, d, 0,  //
            w, 0, h,  //
            w, d, h,  //
        };
        int voxes_second[12] = {
            0, 0, 0,  //
            0, d, 0,  //
            0, 0, h,  //
            0, d, h,  //
        };
        _transform(voxes_first, vox, &draw_obj->basic, 4);
        _transform(voxes_second, vox, &draw_obj->basic, 4);
        struct PolyInfo poly_info = poly_info_create(east_wall_obj, depth, height, inner_info);
        css_base_draw_plane(&poly_info, voxes_first, east_wall_obj);
        css_base_draw_plane(&poly_info, voxes_second, east_wall_obj);
        poly_info_clear(&poly_info);
    }

    struct WallObj *south_wall_obj = obj->south_wall;
    if (south_wall_obj && width > 0 && height > 0) {
        int voxes_first[12] = {
            0, 0, 0,  //
            w, 0, 0,  //
            0, 0, h,  //
            w, 0, h,  //
        };
        int voxes_second[12] = {
            0, d, 0,  //
            w, d, 0,  //
            0, d, h,  //
            w, d, h,  //
        };
        _transform(voxes_first, vox, &draw_obj->basic, 4);
        _transform(voxes_second, vox, &draw_obj->basic, 4);
        struct PolyInfo poly_info = poly_info_create(south_wall_obj, width, height, inner_info);
        css_base_draw_plane(&poly_info, voxes_first, south_wall_obj);
        css_base_draw_plane(&poly_info, voxes_second, south_wall_obj);
        poly_info_clear(&poly_info);
    }

    struct WallObj *roof_obj = obj->roof;
    if (roof_obj && width > 0 && depth > 0) {
        int voxes[12] = {
            0, 0, h,  //
            w, 0, h,  //
            0, d, h,  //
            w, d, h,  //
        };
        _transform(voxes, vox, &draw_obj->basic, 4);
        struct PolyInfo poly_info = poly_info_create(roof_obj, width, depth, inner_info);
        css_base_draw_plane(&poly_info, voxes, roof_obj);
        poly_info_clear(&poly_info);
    }

    int *out_vox = inner_info->out_vox;
    if (out_vox) {
        out_vox[0] = width;
        out_vox[1] = depth;
        out_vox[2] = height;
    }
}
