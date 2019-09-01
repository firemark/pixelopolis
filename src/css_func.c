#include <stdio.h>
#include <string.h>

#include "css_debug.h" 
#include "css_func.h"
#include "css_eval.h"

struct RuleSelector default_selector = {
    .element=NULL,
    .klass=NULL,
    .pseudo_klass=NULL,
};

struct Rule* css_find_rule_by_query(
        struct Program* program, 
        struct RuleSelector* query) {
    struct Rule* rule;
    struct RuleSelector* selector;
    css_iter(rule, program->rules) {
        selector = rule->selector;
        if (
            (!query->element || !strcmp(selector->element, query->element))
            && (!query->klass || !strcmp(selector->klass, query->klass))
            && (!query->pseudo_klass || !strcmp(selector->pseudo_klass, query->pseudo_klass))
           ) {
            return rule;
        }
    }
    return NULL;
}

struct Prop* css_find_prop(struct Rule* rule, char* name) {
    struct Prop* prop;
    css_iter(prop, rule->props) {
        if (!strcmp(prop->name, name)) {
            return prop;
        }
    }
    return NULL;
}

struct Obj* css_find_1st_obj(struct Rule* rule, char* name) {
    struct Prop* prop = css_find_prop(rule, name);
    return prop ? prop->objs[0] : NULL;
}

int* css_find_number_prop(struct Rule* rule, char* name) {
    struct Obj* obj = css_find_1st_obj(rule, name);
    if (!obj) return NULL;
    if (obj->type & OBJ_DYNAMIC) return css_eval_number(obj);
    if (obj->type != OBJ_NUMBER) return NULL;
    return (int*)obj->value;
}

int* css_find_percent_prop(struct Rule* rule, char* name) {
    struct Obj* obj = css_find_1st_obj(rule, name);
    if (!obj) return NULL;
    if (obj->type & OBJ_DYNAMIC) return css_eval_percent(obj);
    if (obj->type != OBJ_PERCENT) return NULL;
    return (int*)obj->value;
}

char* css_find_string_prop(struct Rule* rule, char* name) {
    struct Obj* obj = css_find_1st_obj(rule, name);
    if (!obj) return NULL;
    if (obj->type & OBJ_DYNAMIC) return css_eval_string(obj);
    if (obj->type != OBJ_STRING) return NULL;
    return (char*)obj->value;
}

struct RuleSelector* css_find_selector_prop(struct Rule* rule, char* name) {
    struct Obj* obj = css_find_1st_obj(rule, name);
    if (!obj) return NULL;
    if (obj->type & OBJ_DYNAMIC) return css_eval_rule(obj);
    if (obj->type != OBJ_RULE) return NULL;

    return (struct RuleSelector*)obj->value;
}

struct Rule* css_find_rule_prop(struct Program* program, struct Rule* rule, char* name) {
    struct RuleSelector* selector = css_find_selector_prop(rule, name);
    if (!selector) return NULL;
    return css_find_rule_by_query(program, selector);
}
