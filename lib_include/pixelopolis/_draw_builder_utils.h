#pragma once
#include "pixelopolis/css.h"
#include "pixelopolis/draw_builder_common.h"

enum JustifyIndex { V_JUSTIFY = 0, D_JUSTIFY = 1 };

#define MAKE_SELECTOR_HELPER(helper, prop)                                                      \
    {                                                                                           \
        .program = helper->program, .parent_rule = helper->rule,                                \
        .selector = css_find_selector_prop(helper->rule->rule, prop), .parent = helper->parent, \
    }

int builder_get_int(struct RuleWithParent* rule, char* key, const int default_value);
int builder_get_percent(struct RuleWithParent* rule, char* key, const int default_value);
int builder_get_padding(struct RuleWithParent* rule);
enum Justify builder_get_justify(struct RuleWithParent* rule, char* key,
                                 const enum JustifyIndex index);