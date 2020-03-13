#include <stdlib.h>
#include "_css_draw.h"

void css_draw_cube(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    struct CubeObj *obj = draw_obj->obj;
    if (!obj) return;
    int *vox = inner_info->vox;
    int width = draw_obj->basic.width;
    int height = draw_obj->basic.height;
    int depth = draw_obj->basic.depth;
    int w = width;
    int h = height;
    int d = depth;

    struct WallObj *east_wall_obj = obj->east_wall;
    if (east_wall_obj && depth > 0 && height > 0) {
        int voxes_first[12] = {
            POINT(vox, w, 0, 0),
            POINT(vox, w, d, 0),
            POINT(vox, w, 0, h),
            POINT(vox, w, d, h),
        };
        int voxes_second[12] = {
            POINT(vox, 0, 0, 0),
            POINT(vox, 0, d, 0),
            POINT(vox, 0, 0, h),
            POINT(vox, 0, d, h),
        };
        struct FlatImage* img_to_draw = css_draw_make_texture_from_wall(east_wall_obj, depth, height);
        css_base_draw_plane(inner_info->img, img_to_draw, voxes_first);
        css_base_draw_plane(inner_info->img, img_to_draw, voxes_second);
        free(img_to_draw);
    }

    struct WallObj *south_wall_obj = obj->south_wall;
    if (south_wall_obj && width > 0 && height > 0) {
        int voxes[12] = {
            POINT(vox, 0, 0, 0),
            POINT(vox, w, 0, 0),
            POINT(vox, 0, 0, h),
            POINT(vox, w, 0, h),
        };
        struct FlatImage* img_to_draw = css_draw_make_texture_from_wall(south_wall_obj, width, height);
        css_base_draw_plane(inner_info->img, img_to_draw, voxes);
        free(img_to_draw);
    }

    struct WallObj *roof_obj = obj->roof;
    if (roof_obj && width > 0 && depth > 0) {
        int voxes[12] = {
            POINT(vox, 0, 0, h),
            POINT(vox, w, 0, h),
            POINT(vox, 0, d, h),
            POINT(vox, w, d, h),
        };
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
