#include <stdlib.h>

#include "pixelopolis/_draw_builder.h"
#include "pixelopolis/css_eval.h"
#include "pixelopolis/css_func.h"

static inline const size_t _get_size(struct Obj** prop_objs) {
    size_t size = 0;
    struct Obj* obj = NULL;
    css_iter (obj, prop_objs)
        size++;  // counter
    return size;
}

struct ShiftDrawPair** _build_many_objs(struct SeriesObj* series, struct Helper* helper) {
    struct RuleWithParent* rule = helper->rule;
    enum FillDirection fill_direction = series->fill_direction;
    struct Obj** prop_objs = css_find_objs(rule->rule, "body");
    if (!prop_objs) {
        return NULL;
    }

    const size_t size = _get_size(prop_objs);
    struct BasicObj basic_temp = builder_build_empty_basic();
    struct ShiftDrawPair** pairs = malloc(sizeof(struct ShiftDrawPair*) * (size + 1));
    int shift = 0;

    size_t index = 0;
    struct Obj* obj = NULL;
    css_iter (obj, prop_objs) {
        struct RuleSelector* selector = css_eval_rule(obj);
        struct DrawObj* child = series_make_draw_obj(helper, selector);
        if (!child) continue;

        pairs[index++] = series_make_pair(shift, child);

        int child_width = series_get_basic_metric_by_fill_direction(&child->basic, fill_direction);
        int padding = builder_get_padding(rule);
        series_add_max_basic_by_fill_direction(&basic_temp, &child->basic, fill_direction);
        if (index < size) {
            series_add_basic_metric_by_fill_direction(&basic_temp, fill_direction, padding);
            shift += child_width + padding;
        }
    }

    // resize array
    pairs = realloc(pairs, sizeof(struct ShiftDrawPair*) * (index + 1));
    pairs[index] = NULL;

    series_max_basic(&helper->parent->basic, &basic_temp);  // resize parent

    int end_width =
        series_get_basic_metric_by_fill_direction(&helper->parent->basic, fill_direction);
    series_align_objs(helper, pairs, fill_direction, size, end_width);

    return pairs;
}

struct DrawObj* builder_build_series(struct Helper* helper, enum FillDirection fill_direction) {
    struct RuleWithParent* rule = helper->rule;

    struct SeriesObj* obj = malloc(sizeof(struct SeriesObj));
    obj->fill_direction = fill_direction;

    struct BasicObj basic = builder_build_basic(rule, helper->parent);
    struct DrawObj* draw_obj = builder_make_draw_obj(helper, basic, DRAW_OBJ_SERIES, obj);
    struct Helper inner_helper = {
        .program = helper->program,
        .rule = helper->rule,
        .parent = draw_obj,
    };
    obj->pairs = _build_many_objs(obj, &inner_helper);

    return draw_obj;
}
