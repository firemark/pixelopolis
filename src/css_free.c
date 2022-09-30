#include <stdlib.h>
#include "pixelopolis/hash.h"
#include "pixelopolis/css_func.h"

void css_free_obj(struct Obj* obj) {
    if (obj->type & OBJ_BINARY_OP) {
        css_free_pair_obj((struct PairObj*)obj->value);
    } else switch(obj->type) {
        case OBJ_FUNC: css_free_func_obj((struct FuncObj*)obj->value); break;
        case OBJ_RULE: css_free_rule_selector((struct RuleSelector*)obj->value); break;
        default: free(obj->value); break;
    }
    free(obj);
}

void css_free_func_obj(struct FuncObj* func) {
    size_t i;
    for(i = 0; i < func->args_size; i++) {
        css_free_obj(func->args[i]);
    }
    free(func->name);
    free(func);
}

void css_free_pair_obj(struct PairObj* pair) {
    css_free_obj(pair->left);
    css_free_obj(pair->right);
    free(pair);
}

void css_free_rule_selector(struct RuleSelector* selector) {
    free(selector->element);
    if (selector->klasses) {
        char* klass;
        css_iter(klass, selector->klasses) {
            free(klass);
        }
    }
    free(selector->klasses);
    free(selector->pseudo_klass);
    // if (selector->parent) css_free_rule_selector(selector->parent);
    free(selector);
}

void css_free_objs(struct Obj** objs) {
    return; // currently todo - without gargabage collector is hard to delete sth
    struct Obj* obj;
    css_iter(obj, objs) {
        css_free_obj(obj);
    }
    free(objs);
}

void css_free_rule(struct Rule* rule) {
    struct Obj** objs;
    hash_iter_values(objs, rule->props) {
        css_free_objs(objs);
    }
    hash_destroy(rule->props);
    css_free_rule_selector(rule->selector);
    free(rule);
}

void css_free_rule_half(struct Rule* rule) {
    hash_destroy(rule->props);
    free(rule);
}

void css_free_program(struct Program* program) {
    struct Rule* rule;
    css_iter(rule, program->rules) {
        css_free_rule(rule);
    }
    free(program->name);
    free(program);
}
