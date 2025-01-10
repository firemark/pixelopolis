#include <stdlib.h>

#include "pixelopolis/_css_draw.h"

void css_draw_void(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    struct VoidObj *obj = draw_obj->obj;
    if (!obj) return;
    struct DrawObj *child = obj->child;
    if (!child) return;
    int *out_vox = inner_info->out_vox;

    int vox[3] = COPY_VOX(inner_info->vox);
    _justify_vd(vox, &child->basic, &draw_obj->basic);

    struct DrawInfo draw_info = {
        .img = inner_info->img,
        .vox = vox,
        // .filter = inner_info->filter,
    };
    draw_component(child, &draw_info, out_vox);

    if (out_vox) {
        int width = draw_obj->basic.width;
        int height = draw_obj->basic.height;
        int depth = draw_obj->basic.depth;
        out_vox[0] = width;
        out_vox[1] = depth;
        out_vox[2] = height;
    }
}
