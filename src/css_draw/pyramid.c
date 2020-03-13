#include <stdlib.h>
#include "_css_draw.h"

void css_draw_pyramid(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    struct PyramidObj *obj = draw_obj->obj;
    if (!obj) return;
    int *vox = inner_info->vox;
    int width = draw_obj->basic.width;
    int height = draw_obj->basic.height;
    int depth = draw_obj->basic.depth;
    int w = width;
    int wh = width / 2;
    int dh = depth / 2;
    int h = height;
    int d = depth;

    struct WallObj *south_wall_obj = obj->south_wall;
    if (south_wall_obj && width > 0 && height > 0) {
        struct FlatImage* img_to_draw_first = css_draw_make_texture_from_wall(south_wall_obj, width, height);
        struct FlatImage* img_to_draw_second = css_draw_make_texture_from_wall(south_wall_obj, width, height);

        int voxes_first[9] = {
            POINT(vox, 0 , 0 , 0),
            POINT(vox, w , 0 , 0),
            POINT(vox, wh, dh, h),
        };
        css_base_draw_wide_triangle(inner_info->img, img_to_draw_first, voxes_first);

        int voxes_second[9] = {
            POINT(vox, 0 , d , 0),
            POINT(vox, w , d , 0),
            POINT(vox, wh, dh, h),
        };
        css_base_draw_wide_triangle(inner_info->img, img_to_draw_second, voxes_second);

        free(img_to_draw_first);
        free(img_to_draw_second);
    }

    struct WallObj *east_wall_obj = obj->east_wall;
    if (east_wall_obj && width > 0 && height > 0) {
        struct FlatImage* img_to_draw_first = css_draw_make_texture_from_wall(east_wall_obj, width, height);
        struct FlatImage* img_to_draw_second = css_draw_make_texture_from_wall(east_wall_obj, width, height);

        int voxes_first[9] = {
            POINT(vox, 0 , 0 , 0),
            POINT(vox, 0 , d , 0),
            POINT(vox, wh, dh, h),
        };
        css_base_draw_wide_triangle(inner_info->img, img_to_draw_first, voxes_first);

        int voxes_second[9] = {
            POINT(vox, w , 0 , 0),
            POINT(vox, w , d , 0),
            POINT(vox, wh, dh, h),
        };
        css_base_draw_wide_triangle(inner_info->img, img_to_draw_second, voxes_second);

        free(img_to_draw_first);
        free(img_to_draw_second);
    }

    int *out_vox = inner_info->out_vox;
    if (out_vox) {
        out_vox[0] = width;
        out_vox[1] = depth;
        out_vox[2] = height;
    }
}
