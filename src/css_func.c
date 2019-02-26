#include <stdio.h>
#include <string.h>

#include "css_func.h"

struct Rule* css_find_rule_by_query(
        struct Program* program, 
        struct RuleSelector query) {
    struct Rule* rule;
    css_iter(rule, program->rules) {
        struct RuleSelector* selector = rule->selector;
        if (
            (!query.element || strcmp(selector->element, query.element))
            && (!query.klass || strcmp(selector->klass, query.klass))
            && (!query.pseudo_klass || strcmp(selector->pseudo_klass, query.pseudo_klass))
           ) {
            return rule;
        }
    }
    return NULL;
}

struct Prop* css_find_prop(struct Rule* rule, char* name) {
    struct Prop* prop;
    css_iter(prop, rule->props) {
        if (strcmp(prop->name, name)) {
            return prop;
        }
    }
    return NULL;
}

struct Obj* css_get_first_obj_from_prop(struct Prop* prop) {
    return prop->objs[0];
}
