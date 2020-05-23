#include <stdlib.h>
#include "_draw_builder.h"
#include "css_func.h"

struct DrawObj* builder_build_triangle(struct Helper* helper) {
    struct Rule *rule = helper->rule;
    if (!rule) return NULL;
    struct TriangleObj* obj = malloc(sizeof(struct TriangleObj));
    struct BasicObj basic = builder_build_basic(rule, helper->parent);

    struct SelectorHelper wall_helper = {
        .program=helper->program,
        .parent_rule=rule,
        .selector=css_find_selector_prop(rule, "wall"),
        .parent=helper->parent,
    };
    obj->wall = builder_build_wall(&wall_helper, basic.width, basic.height);

    struct SelectorHelper roof_helper = {
        .program=helper->program,
        .parent_rule=rule,
        .selector=css_find_selector_prop(rule, "roof"),
        .parent=helper->parent,
    };
    const int roof_length = sqrt(
        basic.width * basic.width / 4.0
        + basic.height * basic.height
    );
    obj->roof = builder_build_wall(&roof_helper, basic.depth, roof_length);

    return builder_make_draw_obj(helper, basic, DRAW_OBJ_TRIANGLE, obj);
}
