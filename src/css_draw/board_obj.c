#include <stdlib.h>

#include "pixelopolis/_css_draw.h"
#include "pixelopolis/css_draw.h"

void css_draw_board(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    struct BoardObj *obj = draw_obj->obj;
    if (!obj) return;
    struct BoardChild **children = obj->children;
    if (!children) return;
    struct BasicObj *basic = &draw_obj->basic;

    struct BoardChild *child;
    while ((child = *children++)) {
        if (!child->obj) {
            continue;
        }
        int vox[3] = COPY_VOX(inner_info->vox);
        vox[0] += _x_rotate(child->x, child->y, basic);
        vox[1] += _y_rotate(child->x, child->y, basic);

        struct DrawInfo draw_info = {
            .img = inner_info->img,
            .vox = vox,
            .filter = inner_info->filter,
        };

        draw_component(child->obj, &draw_info, NULL);
    }

    int *out_vox = inner_info->out_vox;
    if (out_vox) {
        int width = draw_obj->basic.width;
        int height = draw_obj->basic.height;
        int depth = draw_obj->basic.depth;
        out_vox[0] = width;
        out_vox[1] = depth;
        out_vox[2] = height;
    }
}
