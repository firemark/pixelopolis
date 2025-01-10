#include <string.h>

#include "pixelopolis/_draw_builder_texture.h"
#include "pixelopolis/css/func.h"

struct TexObj *builder_texture_build_tex_obj(struct SelectorHelper *helper) {
    struct RuleSelector *query = helper->selector;
    if (!query) return NULL;
    struct RuleWithParent *rule = builder_texture_make_rule_from_helper(helper);
    if (!rule) return NULL;

    struct Helper inner_helper = {
        .program = helper->program,
        .rule = rule,
        .parent = helper->parent,
    };

    struct RuleSelector *display_selector = css_find_selector_prop(rule->rule, "display");
    char *element = display_selector ? display_selector->element : query->element;
    struct TexObj *obj;
#define IF_NAME(str) else if (!strcmp(element, str))
    if (!element) obj = NULL;
    // basic
    // containers
    IF_NAME("tile") obj = builder_texture_build_tile(&inner_helper);
    IF_NAME("v-tex") obj = builder_texture_build_texture_part(&inner_helper, TEX_PART_VERTICAL);
    IF_NAME("h-tex") obj = builder_texture_build_texture_part(&inner_helper, TEX_PART_HORIZONTAL);
    IF_NAME("v-series-tex")
    obj = builder_texture_build_texture_series(&inner_helper, TEX_PART_VERTICAL);
    IF_NAME("h-series-tex")
    obj = builder_texture_build_texture_series(&inner_helper, TEX_PART_HORIZONTAL);
    else obj = builder_texture_build_default(&inner_helper);
#undef IF_NAME
    return obj;
}
