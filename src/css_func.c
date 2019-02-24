#include <stdio.h>

#include "css.h"

void css_debug_program(FILE* fp, struct Program* program) {
    fprintf(fp, "PROGRAM %s\n", program->name);
    int i = 0;
    struct Rule* rule;
    while(rule = program->rules[i++]) {
        css_debug_rule(fp, rule);
    }
}

void css_debug_rule(FILE* fp, struct Rule* rule) {
    int i = 0;
    struct Prop* prop;
    fprintf(fp, "%s {\n", rule->name);
    while(prop = rule->props[i++]) {
        css_debug_prop(fp, prop);
    }
    fprintf(fp, "}\n");
}

void css_debug_prop(FILE* fp, struct Prop* prop) {
    fprintf(fp, "    %s: <OBJ>;\n", prop->name);
}
