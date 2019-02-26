#include <stdio.h>

#include "css.h"
#include "css_debug.h"

void css_debug_program(FILE* fp, struct Program* program) {
    fprintf(fp, "PROGRAM %s\n", program->name);
    struct Rule* rule;
    css_iter(rule, program->rules) {
        css_debug_rule(fp, rule);
    }
}

void css_debug_rule(FILE* fp, struct Rule* rule) {
    int i = 0;
    struct Prop* prop;
    css_debug_rule_selector(fp, rule->selector);
    fprintf(fp, " {\n");
    while(prop = rule->props[i++]) {
        css_debug_prop(fp, prop);
        fprintf(fp, ";\n");
    }
    fprintf(fp, "}\n");
}

void css_debug_prop(FILE* fp, struct Prop* prop) {
    int i = 0;
    fprintf(fp, "    %s: ", prop->name);
    struct Obj* obj = prop->objs[i];
    if (!obj) {
        fprintf(fp, "<NULL>");
        return;
    } 

    for(;;) {
        css_debug_obj(fp, obj);
        obj = prop->objs[++i];
        if (!obj) break;
        fprintf(fp, " | ");
    }
}

void css_debug_obj(FILE* fp, struct Obj* obj) {
    switch(obj->type) {
        case OBJ_NUMBER: fprintf(fp, "%d", *((int*)obj->value)); break;
        case OBJ_STRING: fprintf(fp, "\"%s\"", obj->value); break;
        case OBJ_RULE: css_debug_rule_selector(fp, obj->value); break;
        default: fprintf(fp, "???"); break;
    }
}

void css_debug_rule_selector(FILE* fp, struct RuleSelector* selector) {
    if (selector->element) {
        fprintf(fp, "%s", selector->element);
    }
    if (selector->klass) {
        fprintf(fp, ".%s", selector->klass);
    }
    if (selector->pseudo_klass) {
        fprintf(fp, ":%s", selector->pseudo_klass);
    }
}
