#include <string.h>

#include "pixelopolis/_draw_builder_texture.h"
#include "pixelopolis/_draw_builder_texture_part.h"
#include "pixelopolis/css/func.h"

#define SHIFT(pair, direction) pair->shift[builder_texture_get_pair_axis_by_direction(direction)]

static inline void _add_margin_to_tex_objs(struct ShiftTexPair** pairs, const struct IntPair index,
                                           int margin, enum TexPartDirection direction) {
    int _index;
    for (_index = index.start; _index <= index.end; _index++) {
        struct ShiftTexPair* pair = pairs[_index];
        SHIFT(pair, direction) += margin;
    }
}

static inline int _get_margin_to_align(struct ShiftTexPair* pair, int end,
                                       enum TexPartDirection direction) {
    int shift = SHIFT(pair, direction);
    int length = builder_texture_get_metric_by_direction(&pair->obj->basic, direction);
    return end - shift - length;
}

#define IF_NAME(name) else if (!strcmp(align, name))
void builder_texture_align(struct Helper* helper, struct ShiftTexPair** pairs,
                           struct BasicTexObj* basic, struct IntPair length, struct IntPair index,
                           enum TexPartDirection direction) {
    if (index.start > index.end) {
        return;
    }
    int diff = length.end - length.start;
    char* align = css_find_selector_element_prop(helper->rule->rule, "align");
    struct ShiftTexPair* last_pair = pairs[index.end];

    if (!align) return;
    IF_NAME("start") {
        // default is start aligned
        // so we have done a job
    }
    IF_NAME("end") {
        // default is start aligned so we need to
        // find distance between last texture and end of wall
        // and add to shifts
        int margin = _get_margin_to_align(last_pair, diff, direction);
        _add_margin_to_tex_objs(pairs, index, margin, direction);
        builder_texture_resize_axis_by_direction(basic, length.end, direction);
    }
    IF_NAME("center") {
        // default is start aligned so we need to
        // find distance between last texture and end of wall
        // div by 2
        // and add to shifts
        int margin = _get_margin_to_align(last_pair, diff, direction) / 2;
        _add_margin_to_tex_objs(pairs, index, margin, direction);
        builder_texture_resize_axis_by_direction(basic, length.end, direction);
    }
    IF_NAME("justify") {
        // we need to find a last margin and rescale other margins
        // and shift last element to the end
        int end_margin = _get_margin_to_align(last_pair, diff, direction);

        // find a scale of last margin
        int shift = SHIFT(last_pair, direction);
        float ratio = (float)(shift + end_margin) / (float)shift;

        // rescale margins (except last item)
        int _index;
        for (_index = index.start; _index < index.end; _index++) {
            struct ShiftTexPair* next_pair = pairs[_index + 1];
            // move next element to increase margin
            SHIFT(next_pair, direction) *= ratio;
        }
        builder_texture_resize_axis_by_direction(basic, length.end, direction);
    }
}
#undef IF_NAME