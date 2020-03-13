#include <stdlib.h>
#include "_css_draw.h"
#include "css_draw.h"

void css_draw_void(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    struct VoidObj *obj = draw_obj->obj;
    if (!obj) return;
    struct DrawObj *child = obj->child;
    if (!child) return;
    int *out_vox = inner_info->out_vox;

    struct DrawInfo draw_info = {
        .img=inner_info->img,
        .vox=inner_info->vox,
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
