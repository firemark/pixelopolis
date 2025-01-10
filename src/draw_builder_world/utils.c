#include <stdlib.h>
#include <string.h>

#include "pixelopolis/_draw_builder_world.h"
#include "pixelopolis/draw_builder_texture.h"
#include "pixelopolis/css/func.h"

struct RuleWithParent* builder_make_rule_from_helper(struct SelectorHelper* helper) {
    if (!helper->selector) return NULL;
    return css_make_rule_from_selector(helper->program, helper->selector, helper->parent_rule);
}

struct DrawObj* builder_make_draw_obj(struct Helper* helper, struct BasicObj basic,
                                      enum DrawObjType type, void* obj) {
    struct DrawObj* draw_obj = HELPER_ALLOCATE(helper, struct DrawObj);
    draw_obj->basic = basic;
    draw_obj->type = type;
    draw_obj->obj = obj;
    draw_obj->parent = helper->parent;

    return draw_obj;
}


struct TexObj* builder_make_texture(struct SelectorHelper* helper, int width, int height) {
    return builder_texture_make(helper->program, helper->parent_rule, helper->selector, width, height);
}