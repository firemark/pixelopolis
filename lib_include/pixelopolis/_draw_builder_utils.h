#pragma once

struct Helper {
    struct Program* program;
    struct Rule* rule;
    struct DrawObj* parent;
};

struct SelectorHelper {
    struct Program* program;
    struct Rule* parent_rule;
    struct RuleSelector* selector;
    struct DrawObj* parent;
};

enum JustifyIndex { V_JUSTIFY = 0, D_JUSTIFY = 1 };

#define make_selector_helper(helper, prop) { \
    .program=helper->program, \
    .parent_rule=helper->rule, \
    .selector=css_find_selector_prop(helper->rule, prop), \
    .parent=helper->parent, \
}

struct Rule* builder_make_rule_from_helper(struct SelectorHelper* helper);
int builder_get_int(struct Rule* rule, char* key, const int default_value);
int builder_get_percent(struct Rule* rule, char* key, const int default_value);
int builder_get_padding(struct Rule* rule);
enum Justify builder_get_justify(struct Rule* rule, char* key, const enum JustifyIndex index);