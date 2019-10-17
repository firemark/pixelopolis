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
    css_debug_rule_selector(fp, rule->selector);
    fprintf(fp, " {\n");
    struct HashStrItem* item;
    hash_iter(item, rule->props) {
        fprintf(fp, "    %s: ", item->key);
        css_debug_objs(fp, item->value);
        fprintf(fp, ";\n");
    }
    fprintf(fp, "}\n");
}

void css_debug_objs(FILE* fp, struct Obj** objs) {
    int i = 0;
    struct Obj* obj = objs[i];

    if (!obj) {
        fprintf(fp, "<NULL>");
        return;
    } 

    for(;;) {
        css_debug_obj(fp, obj);
        obj = objs[++i];
        if (!obj) break;
        fprintf(fp, " | ");
    }
}

void css_debug_pair(FILE* fp, struct Obj* obj, char op) {
    struct PairObj* pair = (struct PairObj*)obj->value;

    fprintf(fp, "(");
    css_debug_obj(fp, pair->left);
    fprintf(fp, " %c ", op);
    css_debug_obj(fp, pair->right);
    fprintf(fp, ")");
}

void css_debug_func(FILE* fp, struct Obj* obj) {
    struct FuncObj* func = (struct FuncObj*)obj->value;
    struct Obj* arg = func->args[0];
    int i = 0;

    fprintf(fp, "%s(", func->name);
    if (!arg) goto end;
    for(;;) {
        css_debug_obj(fp, arg);
        arg = func->args[++i];
        if (!arg) break;
        fprintf(fp, ", ");
    }
    end: fprintf(fp, ")");
}

void css_debug_obj(FILE* fp, struct Obj* obj) {
    switch(obj->type) {
        case OBJ_NUMBER: fprintf(fp, "%d", *((int*)obj->value)); break;
        case OBJ_PERCENT: fprintf(fp, "%d%%", *((int*)obj->value)); break;
        case OBJ_STRING: fprintf(fp, "\"%s\"", (char*)obj->value); break;
        case OBJ_ADD: css_debug_pair(fp, obj, '+'); break;
        case OBJ_SUB: css_debug_pair(fp, obj, '-'); break;
        case OBJ_MUL: css_debug_pair(fp, obj, '*'); break;
        case OBJ_DIV: css_debug_pair(fp, obj, '/'); break;
        case OBJ_FUNC: css_debug_func(fp, obj); break;
        case OBJ_RULE: css_debug_rule_selector(fp, obj->value); break;
        default: fprintf(fp, "???"); break;
    }
}

void css_debug_rule_selector(FILE* fp, struct RuleSelector* selector) {
    if (selector->parent) {
        css_debug_rule_selector(fp, selector->parent);
        fprintf(fp, " > ");
    }
    if (selector->element) {
        fprintf(fp, "%s", selector->element);
    }
    char* klass;
    css_iter(klass, selector->klasses) {
        fprintf(fp, ".%s", klass);
    }
    if (selector->pseudo_klass) {
        fprintf(fp, ":%s", selector->pseudo_klass);
    }
}
