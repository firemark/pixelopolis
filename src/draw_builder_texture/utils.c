#include <stdlib.h>

#include "pixelopolis/_draw_builder_texture.h"
#include "pixelopolis/css_func.h"

struct Rule* builder_texture_make_rule_from_helper(struct SelectorHelper* helper) {
    if (!helper->selector) return NULL;
    struct RuleSelector* selector = css_cpy_selector(helper->selector);
    selector->parent = helper->parent_rule ? helper->parent_rule->selector : NULL;
    return css_make_rule_from_selector(helper->program, selector);
}

struct TexObj* builder_texture_make_tex_obj(struct Helper* helper, struct BasicTexObj basic,
                                            enum TexObjType type, void* obj) {
    struct TexObj* tex_obj = malloc(sizeof(struct TexObj));
    tex_obj->basic = basic;
    tex_obj->type = type;
    tex_obj->obj = obj;
    tex_obj->parent = helper->parent;

    return tex_obj;
}