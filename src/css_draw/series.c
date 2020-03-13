#include "_css_draw.h"
#include "css_draw.h"

void css_draw_series(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    struct SeriesObj *obj = draw_obj->obj;
    enum FillDirection fill_direction = obj->fill_direction;
    if (!obj) return;
    struct ShiftDrawPair **pairs = obj->pairs;
    if (!pairs) return;

    struct ShiftDrawPair* pair = NULL;
    size_t index = 0;
    while((pair = pairs[index++])) {
        int vox[3] = COPY_VOX(inner_info->vox);
        vox[fill_direction] += pair->shift;
        struct DrawInfo draw_info = {
            .img=inner_info->img,
            .vox=vox,
        };
        draw_component(pair->obj, &draw_info, NULL);
    }

    int *out_vox = inner_info->out_vox;
    if (out_vox) {
        out_vox[0] = draw_obj->basic.width;
        out_vox[1] = draw_obj->basic.depth;
        out_vox[2] = draw_obj->basic.height;
    }
}
