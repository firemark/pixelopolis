#include <stdio.h>
#include <string.h>

#include "css_debug.h"
#include "css_func.h"

#define STACK_MAX_SIZE 512

struct RuleSelector default_selector = {
    .element=NULL,
    .klass=NULL,
    .pseudo_klass=NULL,
};

struct Stack {
    struct Obj* objs[STACK_MAX_SIZE];
    size_t size;
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

struct Stack global_stack = { .size=0 };

void _free_stack() {
    size_t i;
    for(i=0; i < global_stack.size; i++) {
        free_obj(global_stack[i]);
    }
    global_stack.size = 0;
}

struct Obj* _make_obj_in_stack() {
    struct Obj* obj = malloc(sizeof(struct Obj));
    global_stack[global_stack.size++] = obj;
    return obj;
}

struct Obj* _eval_binary_op(struct Obj* obj) {
    struct PairObj* pair = (struct PairObj*)obj->value;
    struct evaled_left = _eval(pair->left);
    struct evaled_right = _eval(pair->right);

    if (!evaled_left || evaled_left->type != OBJ_NUMBER) return NULL;
    if (!evaled_right || evaled_right->type != OBJ_NUMBER) return NULL;

    int left = ((int*)evaled_left->value)*;
    int right = ((int*)evaled_right->value)*;
    int* result = malloc(sizeof(int));

    switch (obj->type) {
        case OBJ_ADD: result* = left + right; break;
        case OBJ_SUB: result* = left - right; break;
        case OBJ_MUL: result* = left * right; break;
        case OBJ_DIV: result* = left / right; break;
        default: result* = 0; break;
    }

    struct Obj* result_obj = _make_obj_in_stack();
    result_obj->type = OBJ_NUMBER;
    result_obj->value = result;

    return result_obj;
}

struct Obj* _eval_func(struct Obj* obj) {
    struct FuncObj* func = (struct FuncObj*)obj->value;
    if (!strcmp(func->name, "random") && func->args_size == 2) {
        return do_random();
    }
    return NULL;
}

struct Obj* _eval(struct Obj* obj) {
    if (obj->type & OBJ_BINARY_OP) return _eval_binary_op(obj);
    if (obj->type == OBJ_FUNC) return NULL;
    return obj;
}

int* _number_eval(struct Obj* obj) {
    _free_stack();
    struct Obj* result = _eval(obj);
    if (!obj || obj->type != OBJ_NUMBER) return NULL;
    return (int*)obj->value;
}

int* css_find_number_prop(struct Rule* rule, char* name) {
    struct Obj* obj = css_find_1st_obj(rule, name);
    if (!obj) return NULL;
    if (obj->type & OBJ_DYNAMIC) return _number_eval(obj);
    if (obj->type != OBJ_NUMBER) return NULL;
    return (int*)obj->value;
}

char* css_find_string_prop(struct Rule* rule, char* name) {
    struct Obj* obj = css_find_1st_obj(rule, name);
    if (!obj || obj->type != OBJ_STRING) {
        return NULL;
    }

    return (char*)obj->value;
}

struct RuleSelector* css_find_selector_prop(struct Rule* rule, char* name) {
    struct Obj* obj = css_find_1st_obj(rule, name);
    if (!obj || obj->type != OBJ_RULE) {
        return NULL;
    }

    return (struct RuleSelector*)obj->value;
}

struct Rule* css_find_rule_prop(struct Program* program, struct Rule* rule, char* name) {
    struct RuleSelector* selector = css_find_selector_prop(rule, name);
    if (!selector) return NULL;
    return css_find_rule_by_query(program, selector);
}
