%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "css.h"

FILE *yyin;
struct Program* global_program;
int lines;
int chars;
int old_chars;
int yylex(void);

struct Prop {
    char* name;
    struct Obj **objs;
};

int yyerror (char* err) {
    fprintf(stderr, "%d:%d %s\n", lines, old_chars, err);
    return 1;
}

int yywrap (void) {
    return 1;
}

char* concat_and_free(char* a, char* b) {
    size_t size = strlen(a) + strlen(b);
    char* out = malloc(sizeof(char) * (size + 1));
    strcpy(out, a);
    strcat(out, b);
    out[size] = 0;
    free(a);
    free(b);
    return out;
}

#define make_array(type, size, first_obj) \
    type **arr = malloc(sizeof(type*) * (size + 1)); \
    size_t i; \
    arr[0] = first_obj; \
    for(i=1; i < size; i++) { \
        arr[i] = NULL; \
    }

#define append_to_array(arr, size, obj) \
    size_t i; \
    for(i=0; i < size; i++) { \
        if (arr[i]) continue; \
        arr[i] = obj; \
        break; \
    }

struct Program* make_program(struct Rule **rules) {
    struct Program *program = malloc(sizeof(struct Program));
    char top[] = "top";
    char* name = malloc(sizeof(top));
    strcpy(name, top);
    program->name = name;
    program->rules = rules;

    return program;
}

struct Rule** concat_rules(struct Rule** a, struct Rule** b) {
    struct Rule* rule;
    size_t a_count = 0;
    css_iter(rule, a) a_count++; // find last index
    css_iter(rule, b) a[a_count++] = rule; // insert rules from b to end of 'a' array
    free(b);
    return a;
}

struct Rule* make_rule(struct RuleSelector* selector, struct Prop **props) {
    struct Rule* rule = malloc(sizeof(struct Rule));
    struct HashMap* map_props = hash_make();

    struct Prop *prop;
    css_iter(prop, props) { // transform array to hashmap
        hash_set(map_props, prop->name, prop->objs, NULL);
    }

    rule->selector = selector;
    rule->props = map_props;
    return rule;
}

struct Rule** make_rules(struct RuleSelector** selectors, struct Prop **props) {
    size_t rule_iter = 0;
    struct Rule** rules = malloc(sizeof(struct Rule*) * (RULE_SELECTORS_SIZE + 1));
    struct RuleSelector* selector;
    css_iter(selector, selectors) {
        rules[rule_iter++] = make_rule(selector, props);
    }

    for(; rule_iter <= RULE_SELECTORS_SIZE; rule_iter++) {
        rules[rule_iter] = NULL;
    }

    struct Prop *prop;
    css_iter(prop, props) { // cleaning
        free(prop);
    }
    free(props);

    return rules;
}

struct Prop* make_prop(char *name, struct Obj **objs) {
    struct Prop *prop = malloc(sizeof(struct Prop));
    prop->name = name;
    prop->objs = objs;
    return prop;
}

struct RuleSelector* make_rule_selector(char* element, char** klasses) {
    struct RuleSelector *rule_selector = malloc(sizeof(struct RuleSelector));
    rule_selector->element = element;
    rule_selector->klasses = klasses;
    rule_selector->pseudo_klass = NULL;
    rule_selector->parent = NULL;
    rule_selector->greedy_parent = NULL;
    return rule_selector;
}

struct Obj* make_obj_as_number(int value) {
    struct Obj* obj = malloc(sizeof(struct Obj));
    int *ptr = malloc(sizeof(int));
    *ptr = value;
    obj->type = OBJ_NUMBER;
    obj->value = (void*)ptr;
    return obj;
}

struct Obj* make_obj_as_percent(int value) {
    struct Obj* obj = malloc(sizeof(struct Obj));
    int *ptr = malloc(sizeof(int));
    *ptr = value;
    obj->type = OBJ_PERCENT;
    obj->value = (void*)ptr;
    return obj;
}

struct Obj* make_obj_as_string(char* string) {
    struct Obj* obj = malloc(sizeof(struct Obj));
    obj->type = OBJ_STRING;
    obj->value = (void*)string;
    return obj;
}

struct Obj* make_obj_as_variable(char* variable) {
    struct Obj* obj = malloc(sizeof(struct Obj));
    obj->type = OBJ_VARIABLE;
    obj->value = (void*)variable;
    return obj;
}

struct Obj* make_obj_as_rule(struct RuleSelector* rule_selector) {
    struct Obj* obj = malloc(sizeof(struct Obj));
    obj->type = OBJ_RULE;
    obj->value = (void*)rule_selector;
    return obj;
}

struct Obj* _make_obj_as_op(enum ObjType type, struct Obj* left, struct Obj* right) {
    struct PairObj* pair = malloc(sizeof(struct PairObj));
    pair->left = left;
    pair->right = right;

    struct Obj* obj = malloc(sizeof(struct Obj));
    obj->type = type;
    obj->value = (void*)pair;

    return obj;
}

#define APPEND_OP(name, op) \
    struct Obj* make_obj_as_ ## name(struct Obj* left, struct Obj* right) {\
        _make_obj_as_op(op, left, right);\
    }

APPEND_OP(add, OBJ_ADD)
APPEND_OP(sub, OBJ_SUB)
APPEND_OP(mul, OBJ_MUL)
APPEND_OP(div, OBJ_DIV)

struct Obj* make_obj_as_func(char* name, struct Obj** args) {
    size_t size = 0;
    while(args[size]) size++; // counter

    struct FuncObj* func = malloc(sizeof(struct FuncObj));
    func->name = name;
    func->args = args;
    func->args_size = size;

    struct Obj* obj = malloc(sizeof(struct Obj));
    obj->type = OBJ_FUNC;
    obj->value = (void*)func;
    return obj;
}

struct Obj* make_obj_as_noargs_func(char* name) {
    struct Obj** args = malloc(sizeof(struct Obj**));
    args[0] = NULL;
    return make_obj_as_func(name, args);
}

%}

%start program
%union {
    char sIndex; // symbol table index
	int number;
    char* string;
    struct Program* programPtr;
    struct Rule* rulePtr;
    struct Prop* propPtr;
    struct Obj* objPtr;
    struct RuleSelector* ruleSelectorPtr;
    struct RuleSelector** ruleSelectorPtrMany;
    struct Rule** rulePtrMany;
    struct Prop** propPtrMany;
    struct Obj** objPtrMany;
    char** strMany;
};

%token
    START_BODY END_BODY START_FUNC END_FUNC
    COLON SEMICOLON PIPE COMMA PERCENT
    ADD_OP SUB_OP MUL_OP DIV_OP PARENT_OP
    SPACE
%token <string> WORD STRING CLASS PSEUDO_CLASS VARIABLE
%token <number> NUMBER
%left ADD_OP SUB_OP
%left MUL_OP DIV_OP
%right START_FUNC END_FUNC
%right START_BODY END_BODY
%type <programPtr> program
%type <rulePtrMany> rule
%type <propPtr> prop
%type <objPtr> obj
%type <rulePtrMany> rules
%type <propPtrMany> props
%type <objPtrMany> objs args
%type <ruleSelectorPtr> rule_selector rule_selector_in_rule
%type <ruleSelectorPtrMany> rule_selectors
%type <strMany> classes

%%
program:
        sp rules { global_program = make_program($2); }
        ;

rules:
        rule { make_array(struct Rule, REGULES_SIZE, NULL); concat_rules(arr, $1); $$ = arr; }
        | rules rule { concat_rules($1, $2); $$ = $1; }
        ;

rule:
        rule_selectors START_BODY sp props END_BODY sp { $$ = make_rules($1, $4); }
        ;

rule_selectors:
        rule_selector_in_rule { make_array(struct RuleSelector, RULE_SELECTORS_SIZE, $1); $$ = arr; }
        | rule_selectors COMMA sp rule_selector_in_rule { append_to_array($1, RULE_SELECTORS_SIZE, $4); $$ = $1; }
        ;

rule_selector:
        WORD sp { $$ = make_rule_selector($1, NULL); }
        | WORD classes sp { $$ = make_rule_selector($1, $2); }
        | classes sp { $$ = make_rule_selector(NULL, $1); }
        ; /* TODO - pseudoklasses */

classes:
        CLASS { make_array(char, KLASSES_SIZE, $1); $$ = arr; }
        | classes CLASS { append_to_array($$, KLASSES_SIZE, $2); }


rule_selector_in_rule:
        rule_selector { $$ = $1; }
        | rule_selector_in_rule PARENT_OP sp rule_selector { $4->parent = $1; $$ = $4; }
        | rule_selector_in_rule rule_selector { $2->greedy_parent = $1; $$ = $2; }
        ;

props:
        prop { make_array(struct Prop, PROPS_SIZE, $1); $$ = arr; }
        | props prop { append_to_array($1, PROPS_SIZE, $2); $$ = $1; }
        ;

prop:
        WORD sp COLON sp objs SEMICOLON sp { $$ = make_prop($1, $5); }
        ;

objs:
        obj { make_array(struct Obj, OBJS_SIZE, $1); $$ = arr; }
        | objs PIPE sp obj { append_to_array($1, OBJS_SIZE, $4); $$ = $1; }
        ;

obj:
        NUMBER sp { $$ = make_obj_as_number($1); }
        | NUMBER PERCENT sp { $$ = make_obj_as_percent($1); }
        | STRING sp { $$ = make_obj_as_string($1); }
        | VARIABLE sp { $$ = make_obj_as_variable($1); }
        | rule_selector { $$ = make_obj_as_rule($1); }
        | obj ADD_OP sp obj { $$ = make_obj_as_add($1, $4); }
        | obj SUB_OP sp obj { $$ = make_obj_as_sub($1, $4); }
        | obj MUL_OP sp obj { $$ = make_obj_as_mul($1, $4); }
        | obj DIV_OP sp obj { $$ = make_obj_as_div($1, $4); }
        | START_FUNC sp obj END_FUNC sp { $$ = $3; }
        | WORD sp START_FUNC sp END_FUNC sp { $$ = make_obj_as_noargs_func($1); }
        | WORD sp START_FUNC sp args END_FUNC sp { $$ = make_obj_as_func($1, $5); }
        ;

args:
        obj { make_array(struct Obj, OBJS_SIZE, $1); $$ = arr; }
        | args COMMA sp obj { append_to_array($1, OBJS_SIZE, $4); $$ = $1; }
        ;

sp: SPACE | sp SPACE | %empty;
%%

struct Program* css_parse_file(char* filename) {
    FILE *stream = fopen(filename, "r");
    struct Program* program = css_parse_file_as_stream(stream);
    fclose(stream);
    return program;
}

struct Program* css_parse_file_as_stream(FILE* stream) {
    yyin = stream;
    yyparse();
    return global_program;
}
