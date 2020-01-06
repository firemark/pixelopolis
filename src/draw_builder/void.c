#include <stdlib.h>
#include "_draw_builder.h"
#include "css_func.h"

struct DrawObj* builder_build_void(struct Helper* helper) {
    struct Rule *rule = helper->rule;
    if (!rule) return NULL;
    struct VoidObj* obj = malloc(sizeof(struct VoidObj));

    struct DrawObj *draw_obj = malloc(sizeof(struct DrawObj));
    draw_obj->basic = builder_build_basic(rule, helper->parent);
    draw_obj->type = DRAW_OBJ_VOID;
    draw_obj->obj = obj;
    draw_obj->parent = helper->parent;

    struct RuleSelector* child_selector = css_find_selector_prop(rule, "child");
    struct SelectorHelper child_helper = {
        .program=helper->program,
        .parent_rule=rule,
        .selector=child_selector,
        .parent=helper->parent,
    };
    obj->child = builder_build_draw_obj(&child_helper);

    return draw_obj;
}
