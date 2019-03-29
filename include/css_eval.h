#pragma once
#include "css.h"

void css_eval_start();
void css_eval_stop();
int* css_eval_number(struct Obj* obj);
