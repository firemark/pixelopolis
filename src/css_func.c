#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "basic.h"
#include "css_debug.h"
#include "css_func.h"
#include "css_eval.h"

struct RuleSelector default_selector = {
    .element=NULL,
    .klasses=NULL,
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
            (!query->element || !selector->element || !strcmp(selector->element, query->element))
            //&& (!query->klass || !strcmp(selector->klass, query->klass))
            && (!query->pseudo_klass || !strcmp(selector->pseudo_klass, query->pseudo_klass))
           ) {
            return rule;
        }
    }
    return NULL;
}

struct Obj** css_find_objs(struct Rule* rule, char* name) {
    return hash_get(rule->props, name);
}

struct Obj* css_find_1st_obj(struct Rule* rule, char* name) {
    struct Obj** objs = css_find_objs(rule, name);
    struct Obj* obj = objs ? objs[0] : NULL;
    if (!obj) return NULL;
    if (obj->type & OBJ_DYNAMIC) return css_eval(obj);
    return obj;
}

int* css_find_number_prop(struct Rule* rule, char* name) {
    struct Obj* obj = css_find_1st_obj(rule, name);
    if (!obj) return NULL;
    if (obj->type != OBJ_NUMBER) return NULL;
    return (int*)obj->value;
}

int* css_find_percent_prop(struct Rule* rule, char* name) {
    struct Obj* obj = css_find_1st_obj(rule, name);
    if (!obj) return NULL;
    if (obj->type != OBJ_PERCENT) return NULL;
    return (int*)obj->value;
}

char* css_find_string_prop(struct Rule* rule, char* name) {
    struct Obj* obj = css_find_1st_obj(rule, name);
    if (!obj) return NULL;
    if (obj->type != OBJ_STRING) return NULL;
    return (char*)obj->value;
}

struct RuleSelector* css_find_selector_prop(struct Rule* rule, char* name) {
    struct Obj* obj = css_find_1st_obj(rule, name);
    if (!obj) return NULL;
    if (obj->type != OBJ_RULE) return NULL;
    return (struct RuleSelector*)obj->value;
}

char* css_find_selector_element_prop(struct Rule* rule, char* name) {
    struct RuleSelector* selector = css_find_selector_prop(rule, name);
    if (!selector) return NULL;
    return selector->element;
}

struct rgb* css_find_color_prop(struct Rule* rule, char* name) {
    struct Obj* obj = css_find_1st_obj(rule, name);
    if (!obj) return NULL;
    if (obj->type != OBJ_COLOR) return NULL;
    return (struct rgb*)obj->value;
}

char* css_cpy_str(char* old) {
    if (old == NULL) {
        return NULL;
    }
    size_t size = strlen(old) + 1;
    char* new = malloc(sizeof(char) * size);
    memcpy(new, old, size);
    return new;
}

struct RuleSelector* css_cpy_selector(struct RuleSelector* old) {
    struct RuleSelector* selector = malloc(sizeof(struct RuleSelector));

    selector->element = css_cpy_str(old->element);
    selector->pseudo_klass = css_cpy_str(old->pseudo_klass);

    if (old->klasses) {
        size_t i;
        selector->klasses = malloc(sizeof(char*) * KLASSES_SIZE);
        for(i = 0; i < KLASSES_SIZE; i++) { // cpy klasses
            selector->klasses[i] = css_cpy_str(old->klasses[i]);
        }
    } else {
        selector->klasses = NULL;
    }

    selector->parent = old->parent ? css_cpy_selector(old->parent) : NULL;
    selector->greedy_parent = old->greedy_parent ? css_cpy_selector(old->greedy_parent) : NULL;
    return selector;
}

struct RuleSelector* css_find_last_parent_selector(struct RuleSelector* selector) {
    if (selector->parent) return css_find_last_parent_selector(selector->parent);
    if (selector->greedy_parent) return css_find_last_parent_selector(selector->greedy_parent);
    return selector;
}

