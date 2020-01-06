#include <stdlib.h>
#include "_draw_builder.h"
#include "css_func.h"

void _append_objs_to_filler(struct Helper* helper, struct SeriesObj* filler, int width) {
    int x = 0;
    int size = 0;
    int padding = filler->padding;
    struct DrawObj** objs = malloc(sizeof(struct DrawObj*) * BUILDER_MAX_ELEMENTS);
    struct Rule* rule = helper->rule;
    enum FillDirection fill_direction = filler->fill_direction;

    struct DrawObj* left = NULL; 
    struct DrawObj* right = NULL;

    struct BasicObj basic_temp = builder_build_empty_basic();

    struct RuleSelector* left_selector = css_find_selector_prop(rule, "left");
    struct SelectorHelper left_helper = {
        .program=helper->program,
        .parent_rule=rule,
        .selector=left_selector,
        .parent=helper->parent,
    };
    left = builder_build_draw_obj(&left_helper);
    if (left) {
        int left_width = builder_get_basic_metric_by_fill_direction(&left->basic, fill_direction);
        if (left_width < width) {
            x += left_width + padding;
            builder_add_max_basic_by_fill_direction(&basic_temp, &left->basic, fill_direction);
            builder_add_basic_metric_by_fill_direction(&basic_temp, fill_direction, padding);
        } else {
            left = NULL;
            right = NULL;
            goto final;
        }
    }

    struct RuleSelector* right_selector = css_find_selector_prop(rule, "right");
    struct SelectorHelper right_helper = {
        .program=helper->program,
        .parent_rule=rule,
        .selector=right_selector,
        .parent=helper->parent,
    };
    right = builder_build_draw_obj(&right_helper);
    if (right) {
        int right_width = builder_get_basic_metric_by_fill_direction(&right->basic, fill_direction);
        if (x + right_width < width) {
            width -= right_width;
            builder_add_max_basic_by_fill_direction(&basic_temp, &right->basic, fill_direction);
        } else {
            right = NULL;
            goto final;
        }
    }

    for(;;) {
        struct RuleSelector* middle_selector = css_find_selector_prop(rule, "body");
        struct SelectorHelper middle_helper = {
            .program=helper->program,
            .parent_rule=rule,
            .selector=middle_selector,
            .parent=helper->parent,
        };
        struct DrawObj* middle = builder_build_draw_obj(&middle_helper);
        int middle_width;
        if (middle) {
            middle_width = builder_get_basic_metric_by_fill_direction(&middle->basic, fill_direction);
        } else {
            middle_width = 12; 
        }
        if (x + middle_width >= width) break;

        if (middle) {
            builder_add_max_basic_by_fill_direction(&basic_temp, &middle->basic, fill_direction);
        } else {
            builder_add_basic_metric_by_fill_direction(&basic_temp, fill_direction, 12);
        }
        builder_add_basic_metric_by_fill_direction(&basic_temp, fill_direction, padding);
        x += middle_width + padding;
        objs[size++] = middle;
    }

final:
    builder_max_basic(&helper->parent->basic, &basic_temp); // resize parent
    objs = realloc(objs, sizeof(struct DrawObj*) * (size + 1));
    objs[size] = NULL;
    filler->objs = objs;
    filler->right = right;
    filler->left = left;
}

struct DrawObj* builder_build_filler(struct Helper* helper, enum FillDirection fill_direction) {
    struct Rule *rule = helper->rule;
    int *padding_ptr = css_find_number_prop(rule, "padding");

    struct SeriesObj* obj = malloc(sizeof(struct SeriesObj));
    obj->padding = padding_ptr? *padding_ptr : 0;
    obj->fill_direction = fill_direction;

    struct DrawObj *draw_obj = malloc(sizeof(struct DrawObj));
    draw_obj->basic = builder_build_basic(rule, helper->parent);
    draw_obj->type = DRAW_OBJ_SERIES;
    draw_obj->obj = obj;
    draw_obj->parent = helper->parent;

    struct Helper inner_helper = {
        .program=helper->program,
        .rule=rule,
        .parent=draw_obj,
    };

    int width = builder_get_basic_metric_by_fill_direction(&draw_obj->basic, fill_direction);
    _append_objs_to_filler(&inner_helper, obj, width);

    return draw_obj;
}
