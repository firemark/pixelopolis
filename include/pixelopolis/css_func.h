#pragma once
#include "css.h"
#include "basic.h"

struct Rule* css_find_rule_by_query(
        struct Program* program,
        struct RuleSelector* query);

struct Obj** css_find_objs(struct Rule* rule, char* name);
struct Obj* css_find_1st_obj(struct Rule* rule, char* name);
int* css_find_number_prop(struct Rule* rule, char* name);
int* css_find_percent_prop(struct Rule* rule, char* name);
char* css_find_string_prop(struct Rule* rule, char* name);
char* css_find_selector_element_prop(struct Rule* rule, char* name);
char* css_cpy_str(struct Memory* memory, char* old);
struct rgb* css_find_color_prop(struct Rule* rule, char* name);
struct RuleSelector* css_find_selector_prop(struct Rule* rule, char* name);
struct RuleSelector* css_cpy_selector(struct Memory* memory, struct RuleSelector* old);
struct RuleSelector* css_find_last_parent_selector(struct RuleSelector* selector);

struct RuleWithParent* css_make_rule_from_selector(struct Program* program, struct RuleSelector* selector, struct RuleWithParent* parent);

void css_free_program(struct Program* program);
