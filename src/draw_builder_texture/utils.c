#include <stdlib.h>

#include "pixelopolis/_draw_builder_texture.h"
#include "pixelopolis/css_func.h"
#include "pixelopolis/draw_builder_common.h"

struct RuleWithParent* builder_texture_make_rule_from_helper(struct SelectorHelper* helper) {
    if (!helper->selector) return NULL;
    return css_make_rule_from_selector(helper->program, helper->selector, helper->parent_rule);
}

static inline int _get_basic_width(struct Helper* helper) {
    int* percent = css_find_percent_prop(helper->rule->rule, "width");
    if (percent) {
        return *percent * helper->parent->basic.width / 100;
    }
    int* number = css_find_number_prop(helper->rule->rule, "width");
    if (number) {
        return *number;
    }
    return 0;
}

static inline int _get_basic_height(struct Helper* helper) {
    int* percent = css_find_percent_prop(helper->rule->rule, "height");
    if (percent) {
        return *percent * helper->parent->basic.height / 100;
    }
    int* number = css_find_number_prop(helper->rule->rule, "height");
    if (number) {
        return *number;
    }
    return 0;
}

struct BasicTexObj builder_texture_prepare_basic(struct Helper* helper)
{
    struct BasicTexObj basic = {
        .width = _get_basic_width(helper),
        .height = _get_basic_height(helper),
    };
    return basic;
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