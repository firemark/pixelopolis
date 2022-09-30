#include <stdlib.h>

#include "pixelopolis/_draw_builder.h"
#include "pixelopolis/css_func.h"

void _append_objs_to_filler(struct Helper* helper, struct SeriesObj* filler, int width) {
    struct Rule* rule = helper->rule;
    enum FillDirection fill_direction = filler->fill_direction;

    struct BasicObj basic_temp = builder_build_empty_basic();
    struct ShiftDrawPair** pairs = malloc(sizeof(struct ShiftDrawPair*) * BUILDER_MAX_ELEMENTS);

    int shift = 0;
    int padding = 0;
    size_t size = 0;
    for (;;) {
        struct RuleSelector* selector = css_find_selector_prop(rule, "body");
        struct DrawObj* child = series_make_draw_obj(helper, selector);
        if (!child) continue;

        int child_width = series_get_basic_metric_by_fill_direction(&child->basic, fill_direction);
        if (shift + child_width >= width) {
            // undo last padding and stop
            series_add_basic_metric_by_fill_direction(&basic_temp, fill_direction, -padding);
            break;
        }

        pairs[size++] = series_make_pair(shift, child);

        padding = builder_get_padding(rule);
        series_add_max_basic_by_fill_direction(&basic_temp, &child->basic, fill_direction);
        series_add_basic_metric_by_fill_direction(&basic_temp, fill_direction, padding);
        shift += child_width + padding;
    }

    series_max_basic(&helper->parent->basic, &basic_temp);  // resize parent

    int end_width =
        series_get_basic_metric_by_fill_direction(&helper->parent->basic, fill_direction);
    series_align_objs(helper, pairs, fill_direction, size, end_width);

    pairs = realloc(pairs, sizeof(struct ShiftDrawPair*) * (size + 1));
    pairs[size] = NULL;
    filler->pairs = pairs;
}

struct DrawObj* builder_build_filler(struct Helper* helper, enum FillDirection fill_direction) {
    struct Rule* rule = helper->rule;
    struct SeriesObj* obj = malloc(sizeof(struct SeriesObj));
    obj->fill_direction = fill_direction;

    struct BasicObj basic = builder_build_basic(rule, helper->parent);
    struct DrawObj* draw_obj = builder_make_draw_obj(helper, basic, DRAW_OBJ_SERIES, obj);

    struct Helper inner_helper = {
        .program = helper->program,
        .rule = rule,
        .parent = draw_obj,
    };

    int width = series_get_basic_metric_by_fill_direction(&draw_obj->basic, fill_direction);
    _append_objs_to_filler(&inner_helper, obj, width);

    return draw_obj;
}
