#pragma once
#include "css.h"

void css_debug_program(FILE* fp, struct Program* program);
void css_debug_rule(FILE* fp, struct Rule* rule);
void css_debug_prop(FILE* fp, struct Prop* prop);
void css_debug_obj(FILE* fp, struct Obj* obj);
void css_debug_rule_selector(FILE* fp, struct RuleSelector* selector);
