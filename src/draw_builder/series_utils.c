#include <stdlib.h>
#include <string.h>

#include "pixelopolis/_draw_builder.h"
#include "pixelopolis/css_func.h"

#define SET_GREATER(attr) a->attr = a->attr >= b->attr ? a->attr : b->attr

void series_add_basic_metric_by_fill_direction(struct BasicObj* basic,
                                               enum FillDirection fill_direction, int x) {
    switch (fill_direction) {
        case X_AXIS_FILL:
            basic->width += x;
            break;
        case HORIZONTAL_FILL:
            basic->height += x;
            break;
        case Y_AXIS_FILL:
            basic->depth += x;
            break;
    }
}

int series_get_basic_metric_by_fill_direction(struct BasicObj* basic,
                                              enum FillDirection fill_direction) {
    switch (fill_direction) {
        case X_AXIS_FILL:
            return basic->width;
        case HORIZONTAL_FILL:
            return basic->height;
        case Y_AXIS_FILL:
            return basic->depth;
        default:
            return 0;
    }
}

void series_add_max_basic_by_fill_direction(struct BasicObj* a, struct BasicObj* b,
                                            enum FillDirection fill_direction) {
    switch (fill_direction) {
        case X_AXIS_FILL:
            a->width += b->width;
            SET_GREATER(height);
            SET_GREATER(depth);
            break;
        case HORIZONTAL_FILL:
            a->height += b->height;
            SET_GREATER(width);
            SET_GREATER(depth);
            break;
        case Y_AXIS_FILL:
            a->depth += b->depth;
            SET_GREATER(width);
            SET_GREATER(height);
            break;
    }
}

void series_max_basic(struct BasicObj* a, struct BasicObj* b) {
    SET_GREATER(width);
    SET_GREATER(height);
    SET_GREATER(depth);
}

struct ShiftDrawPair* series_make_pair(int shift, struct DrawObj* obj) {
    struct ShiftDrawPair* pair = malloc(sizeof(struct ShiftDrawPair));
    pair->obj = obj;
    pair->shift = shift;
    return pair;
}

struct DrawObj* series_make_draw_obj(struct Helper* helper, struct RuleSelector* selector) {
    if (!selector) return NULL;
    struct SelectorHelper inner_helper = {
        .program = helper->program,
        .parent_rule = helper->rule,
        .selector = selector,
        .parent = helper->parent,
    };
    struct DrawObj* child = builder_build_draw_obj(&inner_helper);
    if (!child) return NULL;
    return child;
}

int _get_margin_to_align(struct ShiftDrawPair* pair, int end_width,
                         enum FillDirection fill_direction) {
    int width = series_get_basic_metric_by_fill_direction(&pair->obj->basic, fill_direction);
    return end_width - (pair->shift + width);
}

#define IF_NAME(name) else if (!strcmp(align, name))
void series_align_objs(struct Helper* helper, struct ShiftDrawPair** pairs,
                       enum FillDirection fill_direction, size_t size, int end_width) {
    if (size == 0) {
        return;
    }
    char* align = css_find_selector_element_prop(helper->rule, "align");
    size_t end_index = size - 1;
    struct ShiftDrawPair* last_pair = pairs[end_index];

    if (!align) return;  // default is start
    IF_NAME("end") {
        // we need to find distance between last DrawObj and end of series
        // and add to shifts
        int margin = _get_margin_to_align(last_pair, end_width, fill_direction);
        size_t index;
        for (index = 0; index < size; index++) pairs[index]->shift += margin;
    }
    IF_NAME("center") {
        // we need to find distance between last DrawObj and end of series
        // div by 2
        // and add to shifts
        int margin = _get_margin_to_align(last_pair, end_width, fill_direction) / 2;
        size_t index;
        for (index = 0; index < size; index++) pairs[index]->shift += margin;
    }
    IF_NAME("stretch") {
        // we need to find a last margin and rescale other margins
        // and shift last element to the end
        int margin = _get_margin_to_align(last_pair, end_width, fill_direction);
        int shift = last_pair->shift;
        // find a scale of last margin
        float ratio = (float)(shift + margin) / (float)shift;

        // rescale margins
        size_t index;
        for (index = 0; index < end_index; index++) {
            struct ShiftDrawPair* next_pair = pairs[index + 1];
            // move next element to increase margin
            next_pair->shift *= ratio;
        }
        return;
    }
}
#undef IF_NAME
