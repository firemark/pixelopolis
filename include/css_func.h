#pragma once
#include "css.h"

struct Rule* css_find_rule_by_query(
        struct Program* program, 
        struct RuleSelector* query);

struct Obj** css_find_objs(struct Rule* rule, char* name);
struct Obj* css_find_1st_obj(struct Rule* rule, char* name);
int* css_find_number_prop(struct Rule* rule, char* name);
int* css_find_percent_prop(struct Rule* rule, char* name);
char* css_find_string_prop(struct Rule* rule, char* name);
char* css_find_selector_element_prop(struct Rule* rule, char* name);
struct RuleSelector* css_find_selector_prop(struct Rule* rule, char* name);
struct RuleSelector* css_cpy_selector(struct RuleSelector* old);

struct Rule* css_make_rule_from_selector(struct Program* program, struct RuleSelector* selector);

void css_free_obj(struct Obj* obj);
void css_free_func_obj(struct FuncObj* func);
void css_free_pair_obj(struct PairObj* pair);
void css_free_objs(struct Obj** objs);
void css_free_rule(struct Rule* rule);
void css_free_rule_half(struct Rule* rule);
void css_free_rule_selector(struct RuleSelector* selector);
void css_free_program(struct Program* program);
