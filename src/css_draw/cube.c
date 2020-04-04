#include <stdlib.h>
#include "_css_draw.h"

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
            w, 0, 0,
            w, d, 0,
            w, 0, h,
            w, d, h,
        };
        int voxes_second[12] = {
            0, 0, 0,
            0, d, 0,
            0, 0, h,
            0, d, h,
        };
        _transform(voxes_first, vox, &draw_obj->basic, 4);
        _transform(voxes_second, vox, &draw_obj->basic, 4);
        struct FlatImage* img_to_draw = css_draw_make_texture_from_wall(east_wall_obj, depth, height);
        css_base_draw_plane(inner_info->img, img_to_draw, voxes_first);
        css_base_draw_plane(inner_info->img, img_to_draw, voxes_second);
        free(img_to_draw);
    }

    struct WallObj *south_wall_obj = obj->south_wall;
    if (south_wall_obj && width > 0 && height > 0) {
        int voxes[12] = {
            0, 0, 0,
            w, 0, 0,
            0, 0, h,
            w, 0, h,
        };
        _transform(voxes, vox, &draw_obj->basic, 4);
        struct FlatImage* img_to_draw = css_draw_make_texture_from_wall(south_wall_obj, width, height);
        css_base_draw_plane(inner_info->img, img_to_draw, voxes);
        free(img_to_draw);
    }

    struct WallObj *roof_obj = obj->roof;
    if (roof_obj && width > 0 && depth > 0) {
        int voxes[12] = {
            0, 0, h,
            w, 0, h,
            0, d, h,
            w, d, h,
        };
        _transform(voxes, vox, &draw_obj->basic, 4);
        struct FlatImage* img_to_draw = css_draw_make_texture_from_wall(roof_obj, width, depth);
        css_base_draw_plane(inner_info->img, img_to_draw, voxes);
        free(img_to_draw);
    }

    int *out_vox = inner_info->out_vox;
    if (out_vox) {
        out_vox[0] = width;
        out_vox[1] = depth;
        out_vox[2] = height;
    }
}
