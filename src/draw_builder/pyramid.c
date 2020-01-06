#include <stdlib.h>
#include "_draw_builder.h"
#include "css_func.h"

struct DrawObj* builder_build_pyramid(struct Helper* helper) {
    struct Rule *rule = helper->rule;
    if (!rule) return NULL;
    struct PyramidObj *obj = malloc(sizeof(struct PyramidObj));
    struct BasicObj basic = builder_build_basic(rule, helper->parent);

    struct SelectorHelper wall_helper = {
        .program=helper->program,
        .parent_rule=rule,
        .selector=css_find_selector_prop(rule, "wall"),
        .parent=helper->parent,
    };
    obj->south_wall = builder_build_wall(&wall_helper, basic.width, basic.height);
    obj->east_wall = builder_build_wall(&wall_helper, basic.depth, basic.height);

    return builder_make_draw_obj(helper, basic, DRAW_OBJ_PYRAMID, obj);
}
