#include <stdlib.h>
#include "_draw_builder.h"
#include "css_func.h"

struct DrawObj* builder_build_pyramid(struct Helper* helper) {
    struct Rule *rule = helper->rule;
    if (!rule) return NULL;
    struct PyramidObj *obj = malloc(sizeof(struct PyramidObj));
    struct BasicObj basic = builder_build_basic(rule, helper->parent);
    struct RuleSelector* selector = css_find_selector_prop(rule, "wall");
    if (!selector) {
        selector = css_find_selector_prop(rule, "roof");
    }

    struct SelectorHelper wall_helper = {
        .program=helper->program,
        .parent_rule=rule,
        .selector=selector,
        .parent=helper->parent,
    };

    const int south_length = sqrt(
        basic.depth * basic.depth / 4.0
        + basic.height * basic.height
    );
    obj->south_wall = builder_build_wall(&wall_helper, basic.width, south_length);

    const int east_length = sqrt(
        basic.width * basic.width / 4.0
        + basic.height * basic.height
    );
    obj->east_wall = builder_build_wall(&wall_helper, basic.depth, east_length);

    return builder_make_draw_obj(helper, basic, DRAW_OBJ_PYRAMID, obj);
}
