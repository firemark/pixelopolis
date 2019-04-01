#pragma once
#include "css.h"

void css_eval_start();
void css_eval_stop();

struct Obj* css_eval(struct Obj* obj);
int* css_eval_number(struct Obj* obj);
char* css_eval_string(struct Obj* obj);
struct RuleSelector* css_eval_rule(struct Obj* obj);
