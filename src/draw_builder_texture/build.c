#include <string.h>

#include "pixelopolis/_draw_builder_texture.h"
#include "pixelopolis/css_func.h"

struct TexObj* builder_texture_build(struct TextureSelectorHelper* helper) {
    struct Rule* rule = builder_texture_make_rule_from_helper(helper);
    if (!rule) return NULL;

    struct RuleSelector* pattern_selector = css_find_selector_prop(rule, "pattern");
    char* pattern = pattern_selector ? pattern_selector->element : NULL;

    struct TextureHelper inner_helper = {
        .program = helper->program,
        .rule = rule,
        .parent = helper->parent,
    };

    struct TexObj* obj;
#define IF_NAME(str) else if (!strcmp(pattern, str))
    if (!pattern) obj = builder_texture_build_default(&inner_helper);
    IF_NAME("tiles") obj = builder_texture_build_tiles(&inner_helper);
    // IF_NAME("bricks") obj = builder_texture_build_bricks(&inner_helper);
    else obj = builder_texture_build_default(&inner_helper);
#undef IF_NAME

    css_free_rule_half(rule);
    return obj;
}
