#include <stdlib.h>
#include <string.h>

#include "pixelopolis/_draw_builder_texture.h"
#include "pixelopolis/css_func.h"
#include "pixelopolis/draw_builder_common.h"
#include "pixelopolis/draw_builder_texture.h"

static inline void _add_margin_to_tex_objs(struct TexPartObj* obj, int index, int end_index,
                                           int margin, enum TexPartDirection direction) {
    for (; index <= end_index; index++) {
        struct ShiftTexPair* pair = obj->objs[index];
        pair->shift[builder_texture_get_pair_axis_by_direction(direction)] += margin;
    }
}

static inline int _get_margin_to_align(struct ShiftTexPair* pair, int end,
                                       enum TexPartDirection direction) {
    int shift = pair->shift[builder_texture_get_pair_axis_by_direction(direction)];
    int length = builder_texture_get_metric_by_direction(&pair->obj->basic, direction);
    return end - shift - length;
}

#define IF_NAME(name) else if (!strcmp(align, name))
void builder_texture_align(struct Helper* helper, struct TexPartObj* obj, struct BasicTexObj* basic,
                           int start, int end, int start_index, int end_index,
                           enum TexPartDirection direction) {
    if (start_index > end_index) {
        return;
    }
    char* align = css_find_selector_element_prop(helper->rule, "align");
    struct ShiftTexPair* last_pair = obj->objs[end_index];

    if (!align) return;
    IF_NAME("start") {
        // default is start aligned
        // so we have done a job
    }
    IF_NAME("end") {
        // default is start aligned so we need to
        // find distance between last texture and end of wall
        // and add to shifts
        int margin = _get_margin_to_align(last_pair, end - start, direction);
        _add_margin_to_tex_objs(obj, start_index, end_index, margin, direction);
        builder_texture_resize_axis_by_direction(basic, end, direction);
    }
    IF_NAME("center") {
        // default is start aligned so we need to
        // find distance between last texture and end of wall
        // div by 2
        // and add to shifts
        int margin = _get_margin_to_align(last_pair, end - start, direction) / 2;
        _add_margin_to_tex_objs(obj, start_index, end_index, margin, direction);
        builder_texture_resize_axis_by_direction(basic, end, direction);
    }
    IF_NAME("justify") {
        // we need to find a last margin and rescale other margins
        // and shift last element to the end
        int end_margin = _get_margin_to_align(last_pair, end - start, direction);

        // find a scale of last margin
        int shift = last_pair->shift[builder_texture_get_pair_axis_by_direction(direction)];
        float ratio = (float)(shift + end_margin) / (float)shift;

        // rescale margins (except last item)
        size_t index;
        for (index = start_index; index < end_index; index++) {
            struct ShiftTexPair* next_pair = obj->objs[index + 1];
            // move next element to increase margin
            next_pair->shift[builder_texture_get_pair_axis_by_direction(direction)] *= ratio;
        }
        builder_texture_resize_axis_by_direction(basic, end, direction);
    }
}
#undef IF_NAME