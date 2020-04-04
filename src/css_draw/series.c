#include "_css_draw.h"
#include "css_draw.h"

static inline void _update_shift(int vox[3], struct DrawObj *draw_obj, int shift) {
    struct SeriesObj *obj = draw_obj->obj;

    switch(obj->fill_direction) {
        case VERTICAL_FILL:
            vox[0] += shift * draw_obj->basic.cos_th;
            vox[1] += shift * draw_obj->basic.sin_th;
            break;
        case DEPTH_FILL:
            // rotate 90°, reduced trygonometry functions
            vox[0] += shift * -draw_obj->basic.sin_th;
            vox[1] += shift * draw_obj->basic.cos_th;
            break;
        case HORIZONTAL_FILL:
            vox[2] += shift;
            return;
    }
}

void css_draw_series(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    struct SeriesObj *obj = draw_obj->obj;
    if (!obj) return;
    struct ShiftDrawPair **pairs = obj->pairs;
    if (!pairs) return;

    struct ShiftDrawPair* pair = NULL;
    size_t index = 0;
    while((pair = pairs[index++])) {
        int vox[3] = COPY_VOX(inner_info->vox);
        _update_shift(vox, draw_obj, pair->shift);
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
