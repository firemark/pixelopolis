#include <stdlib.h>
#include <string.h>

#include "pixelopolis/_draw_builder_texture.h"
#include "pixelopolis/css_func.h"

struct Rule* builder_texture_make_rule_from_helper(struct TextureSelectorHelper* helper) {
    if (!helper->selector) return NULL;
    struct RuleSelector* selector = css_cpy_selector(helper->selector);
    selector->parent = helper->parent_rule ? helper->parent_rule->selector : NULL;
    return css_make_rule_from_selector(helper->program, selector);
}

/*
int builder_texture_get_int(struct Rule* rule, char* key, const int default_value) {
    int* ptr = css_find_number_prop(rule, key);
    return ptr ? *ptr : default_value;
}
*/