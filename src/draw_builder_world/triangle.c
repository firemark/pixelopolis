#include <stdlib.h>

#include "pixelopolis/_draw_builder_world.h"
#include "pixelopolis/css/func.h"

struct DrawObj* builder_build_triangle(struct Helper* helper) {
    struct RuleWithParent* rule = helper->rule;
    if (!rule) return NULL;
    struct TriangleObj* obj = HELPER_ALLOCATE(helper, struct TriangleObj);
    struct BasicObj basic = builder_build_basic(rule, helper->parent);

    struct SelectorHelper wall_helper = {
        .program = helper->program,
        .parent_rule = rule,
        .selector = css_find_selector_prop(rule->rule, "wall"),
        .parent = helper->parent,
    };
    obj->wall = builder_build_wall(&wall_helper, basic.width, basic.height);

    struct SelectorHelper roof_helper = {
        .program = helper->program,
        .parent_rule = rule,
        .selector = css_find_selector_prop(rule->rule, "roof"),
        .parent = helper->parent,
    };
    const int roof_length = sqrt(basic.width * basic.width / 4.0 + basic.height * basic.height);
    obj->roof = builder_build_wall(&roof_helper, basic.depth, roof_length);

    return builder_make_draw_obj(helper, basic, DRAW_OBJ_TRIANGLE, obj);
}
