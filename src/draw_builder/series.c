#include <stdlib.h>
#include "_draw_builder.h"
#include "css_func.h"
#include "css_eval.h"

struct DrawObj** _build_many_objs(struct SeriesObj* series, struct Helper* helper) {
    struct Rule *rule = helper->rule;
    struct Obj **prop_objs = css_find_objs(rule, "body");
    if (!prop_objs) {
        struct DrawObj **objs = malloc(sizeof(struct DrawObj*));
        objs[0] = NULL;
        return objs;
    }
    struct Obj* obj = NULL;
    int size = 0;
    css_iter(obj, prop_objs) size++; // counter

    struct BasicObj basic_temp = builder_build_empty_basic();
    struct DrawObj **objs = malloc(sizeof(struct DrawObj*) * (size + 1));
    int i = 0;
    css_iter(obj, prop_objs) {
        struct RuleSelector* selector = css_eval_rule(obj);
        if (!selector) continue;

        struct SelectorHelper inner_helper = {
            .program=helper->program,
            .parent_rule=rule,
            .selector=selector,
            .parent=helper->parent,
        };
        struct DrawObj *child = builder_build_draw_obj(&inner_helper);
        if (!child) continue;
        objs[i++] = child;
        builder_add_max_basic_by_fill_direction(&basic_temp, &child->basic, series->fill_direction);
    }

    objs = realloc(objs, sizeof(struct DrawObj*) * (i + 1));
    objs[i] = NULL;

    builder_max_basic(&helper->parent->basic, &basic_temp); // resize parent
    return objs;
}

struct DrawObj* builder_build_series(struct Helper* helper, enum FillDirection fill_direction) {
    struct Rule *rule = helper->rule;
    int *padding_ptr = css_find_number_prop(rule, "padding");

    struct SeriesObj* obj = malloc(sizeof(struct SeriesObj));
    obj->padding = padding_ptr? *padding_ptr : 0;
    obj->fill_direction = fill_direction;
    obj->left = NULL;
    obj->right = NULL;

    struct DrawObj *draw_obj = builder_make_draw_obj(helper, builder_build_basic(rule, helper->parent), DRAW_OBJ_SERIES, obj);
    struct Helper inner_helper = {
        .program=helper->program,
        .rule=helper->rule,
        .parent=draw_obj,
    };
    obj->objs = _build_many_objs(obj, &inner_helper);

    return draw_obj;
}
