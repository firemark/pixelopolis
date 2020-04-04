#include <stdlib.h>
#include "_draw_builder.h"
#include "css_func.h"

struct DrawObj* builder_build_void(struct Helper* helper) {
    struct Rule *rule = helper->rule;
    if (!rule) return NULL;
    struct BasicObj basic = builder_build_basic(rule, helper->parent);
    struct RuleSelector* child_selector = css_find_selector_prop(rule, "child");
    return builder_build_custom_void(helper, basic, child_selector); 
}

struct DrawObj* builder_build_custom_void(
        struct Helper* helper, 
        struct BasicObj basic,
        struct RuleSelector* child_selector) {
    struct VoidObj* obj = malloc(sizeof(struct VoidObj));
    struct DrawObj* draw_obj = builder_make_draw_obj(helper, basic, DRAW_OBJ_VOID, obj);
    struct SelectorHelper child_helper = {
        .program=helper->program,
        .parent_rule=helper->rule,
        .selector=child_selector,
        .parent=draw_obj,
    };
    obj->child = builder_build_draw_obj(&child_helper);
    return draw_obj;
}
