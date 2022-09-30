#include "pixelopolis/_css_draw.h"

static inline void _update_shift(
        int vox[3],
        const struct DrawObj *draw_obj,
        const struct ShiftDrawPair *pair) {
    const int shift = pair->shift;
    const struct DrawObj *child_obj = pair->obj;
    struct SeriesObj *obj = draw_obj->obj;

    switch(obj->fill_direction) {
        case X_AXIS_FILL:
            vox[0] += shift * draw_obj->basic.cos_th;
            vox[1] += shift * draw_obj->basic.sin_th;
            _justify_d(vox, &child_obj->basic, &draw_obj->basic);
            break;
        case Y_AXIS_FILL:
            // rotate 90°, reduced trygonometry functions
            vox[0] += shift * -draw_obj->basic.sin_th;
            vox[1] += shift * draw_obj->basic.cos_th;
            _justify_v(vox, &child_obj->basic, &draw_obj->basic);
            break;
        case HORIZONTAL_FILL:
            vox[2] += shift;
            _justify_vd(vox, &child_obj->basic, &draw_obj->basic);
            break;
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
        _update_shift(vox, draw_obj, pair);
        struct DrawInfo draw_info = {
            .img=inner_info->img,
            .vox=vox,
            .filter=inner_info->filter,
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
