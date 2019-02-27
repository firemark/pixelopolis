#pragma once
#include "css.h"

struct Rule* css_find_rule_by_query(
        struct Program* program, 
        struct RuleSelector* query);
struct Prop* css_find_prop(struct Rule* rule, char* name);
struct Obj* css_find_1st_obj(struct Rule* rule, char* name);
int* css_find_number_prop(struct Rule* rule, char* name);
char* css_find_string_prop(struct Rule* rule, char* name);
struct RuleSelector* css_find_selector_prop(struct Rule* rule, char* name);
struct Rule* css_find_rule_prop(struct Program* program, struct Rule* rule, char* name);
