#include <stdlib.h>
#include "css_func.h"

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
    free(selector->klass);
    free(selector->pseudo_klass);
    free(selector);
}

void css_free_prop(struct Prop* prop) {
    struct Obj* obj;
    css_iter(obj, prop->objs) {
        css_free_obj(obj);
    }
    free(prop->name);
    free(prop);
}

void css_free_rule(struct Rule* rule) {
    struct Prop* prop;
    css_iter(prop, rule->props) {
        css_free_prop(prop);
    }
    css_free_rule_selector(rule->selector);
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
