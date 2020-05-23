#include <stdlib.h>
#include "_css_draw.h"

void css_draw_triangle(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    const struct TriangleObj *obj = draw_obj->obj;
    if (!obj) return;
    const int *vox = inner_info->vox;
    const int width = draw_obj->basic.width;
    const int height = draw_obj->basic.height;
    const int depth = draw_obj->basic.depth;
    const int w = width;
    const int wh = width / 2;
    const int h = height;
    const int d = depth;

    struct WallObj *wall_obj = obj->wall;
    if (wall_obj && width > 0 && height > 0) {
        int voxes[9] = {
            0 , 0, 0,
            w , 0, 0,
            wh, 0, h,
        };
        _transform(voxes, vox, &draw_obj->basic, 3);
        struct FlatImage* img_to_draw = css_draw_make_texture_from_wall(wall_obj, width, height);
        css_base_draw_wide_triangle(inner_info->img, img_to_draw, voxes, wall_obj);
        free(img_to_draw);
    }

    struct WallObj *roof_obj = obj->roof;
    if (roof_obj) {
        struct FlatImage* img_to_draw_first = css_draw_make_texture_from_wall(roof_obj, roof_obj->width, roof_obj->height);
        struct FlatImage* img_to_draw_second = css_draw_make_texture_from_wall(roof_obj, roof_obj->width, roof_obj->height);

        int voxes_first[12] = {
            w , 0, 0,
            w , d, 0,
            wh, 0, h,
            wh, d, h,
        };
        int voxes_second[12] = {
            0 , 0, 0,
            0 , d, 0,
            wh, 0, h,
            wh, d, h,
        };

        _transform(voxes_first, vox, &draw_obj->basic, 4);
        _transform(voxes_second, vox, &draw_obj->basic, 4);

        css_base_draw_plane(inner_info->img, img_to_draw_first, voxes_first, roof_obj);
        css_base_draw_plane(inner_info->img, img_to_draw_second, voxes_second, roof_obj);

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
