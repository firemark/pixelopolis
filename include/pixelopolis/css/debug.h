#pragma once
#include "pixelopolis/css.h"

void css_debug_program(FILE* fp, struct Program* program);
void css_debug_rule(FILE* fp, struct Rule* rule);
void css_debug_objs(FILE* fp, struct Obj** objs);
void css_debug_obj(FILE* fp, struct Obj* obj);
void css_debug_rule_selector(FILE* fp, struct RuleSelector* selector);
