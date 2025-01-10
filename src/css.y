%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "pixelopolis/css.h"
#include "pixelopolis/css/func.h"
#include "pixelopolis/basic.h"
#include "pixelopolis/memory.h"
#define YYDEBUG 1

static struct Program* global_program;

extern FILE *yyin;
extern int lines;
extern int chars;
extern int old_chars;
extern struct Memory* global_memory;
int yylex(void);

enum RuleAttrType {
    RULE_ATTR_PROP, // struct Prop*
    RULE_ATTR_RULE, // struct Rule**
};

struct RuleAttr {
    enum RuleAttrType type;
    void* obj;
};

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

#define make_array(type, size, first_obj) \
    type **arr = MEMORY_ALLOCATE_ARRAY(global_memory, type*, size + 1); \
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
    static char top[] = "top";
    char* name = malloc(sizeof(top));
    strcpy(name, top);
    program->name = name;
    program->rules = rules;
    program->syntax_memory = global_memory;
    program->product_memory = memory_create(32 * 1024 * 1024);

    return program;
}

struct Rule** concat_rules(struct Rule** a, struct Rule** b) {
    struct Rule* rule;
    size_t a_count = 0;
    css_iter(rule, a) a_count++; // find last index
    css_iter(rule, b) a[a_count++] = rule; // insert rules from b to end of 'a' array
    return a;
}

struct Rule* make_rule(struct RuleSelector* selector, struct RuleAttr **attrs) {
    struct Rule* rule = MEMORY_ALLOCATE(global_memory, struct Rule);
    struct HashMap* map_props = hash_make_with_memory(global_memory);

    struct RuleAttr *attr;
    css_iter(attr, attrs) { // transform array to hashmap
        struct Prop *prop;
        switch (attr->type) {
            case RULE_ATTR_PROP:
                prop = attr->obj;
                hash_set(map_props, prop->name, prop->objs, NULL);
                break;
            default: break;
        }
    }

    rule->selector = selector;
    rule->props = map_props;
    return rule;
}

struct RuleAttr* make_rule_attr(void* obj, enum RuleAttrType type) {
    struct RuleAttr* rule_attr = MEMORY_ALLOCATE(global_memory, struct RuleAttr);
    rule_attr->obj = obj;
    rule_attr->type = type;
    return rule_attr;
}

char find_and_replace_parent_op_with_parent_rule(struct RuleSelector**, struct RuleSelector*);

void _cpy_classes_for_parent_op(
        struct RuleSelector* selector,
        struct RuleSelector* copied_selector) {
    if (!selector->klasses) return;

    if (!copied_selector->klasses) {
        make_array(char, KLASSES_SIZE, NULL);
        copied_selector->klasses = arr;
    }

    size_t copied_klass_len = 0;
    while(copied_selector->klasses[copied_klass_len]) {
        copied_klass_len++;
    };

    size_t i = 0;
    for(i = 0; i < KLASSES_SIZE; i++) {
        char* klass = css_cpy_str(global_memory, selector->klasses[i]);
        if (!klass) break;
        copied_selector->klasses[copied_klass_len++] = klass;
    }
}

void _replace_parent_op(
        struct RuleSelector** selector_pointer,
        struct RuleSelector* selector_to_replace) {
    struct RuleSelector* selector = *selector_pointer;
    struct RuleSelector* copied_selector = css_cpy_selector(global_memory, selector_to_replace);
    struct RuleSelector* parent_copied_selector = css_find_last_parent_selector(copied_selector);

    _cpy_classes_for_parent_op(selector, copied_selector);

    if (selector->parent) {
        parent_copied_selector->parent = selector->parent;
        find_and_replace_parent_op_with_parent_rule(
            &parent_copied_selector->parent,
            selector_to_replace);
    }

    if (selector->greedy_parent) {
        parent_copied_selector->greedy_parent = selector->greedy_parent;
        find_and_replace_parent_op_with_parent_rule(
            &parent_copied_selector->greedy_parent,
            selector_to_replace);
    }

    *selector_pointer = copied_selector;
    // css_free_rule_selector(selector);
}

char find_and_replace_parent_op_with_parent_rule(
        struct RuleSelector** selector_pointer,
        struct RuleSelector* selector_to_replace) {
    struct RuleSelector* selector = *selector_pointer;
    char* element = selector->element;

    if (element && strcmp(element, "&") == 0) {
        _replace_parent_op(
            selector_pointer,
            selector_to_replace);
        return 1;
    }

    if (selector->parent) {
        return find_and_replace_parent_op_with_parent_rule(
            &selector->parent,
            selector_to_replace);
    }

    if (selector->greedy_parent) {
        return find_and_replace_parent_op_with_parent_rule(
            &selector->greedy_parent,
            selector_to_replace);
    }

    return 0;
}


struct Rule** unpack_rules(struct RuleSelector* selector, struct RuleAttr **attrs) {
    size_t rule_iter = 0;
    struct Rule** rules = MEMORY_ALLOCATE_ARRAY(global_memory, struct Rule*, RULE_SELECTORS_SIZE + 1);
    rules[rule_iter++] = make_rule(selector, attrs);

    struct RuleAttr *attr;
    css_iter(attr, attrs) { // unpack nested rules
        if (attr->type != RULE_ATTR_RULE) continue;

        struct Rule **nested_rules = attr->obj;
        struct Rule *rule;
        css_iter(rule, nested_rules) {
            assert(rule_iter <= RULE_SELECTORS_SIZE);
            char is_changed = find_and_replace_parent_op_with_parent_rule(
                &rule->selector,
                selector);
            if (!is_changed) { // add parent rule to the end
                struct RuleSelector* parent = css_find_last_parent_selector(rule->selector);
                parent->greedy_parent = css_cpy_selector(global_memory, selector);
            }
            rules[rule_iter++] = rule;
        }
    }
    rules[rule_iter] = NULL;
    return rules;
}


struct Rule** make_rules(struct RuleSelector** selectors, struct RuleAttr **attrs) {
    size_t rule_iter = 0;
    struct Rule** rules = MEMORY_ALLOCATE_ARRAY(global_memory, struct Rule*, RULE_SELECTORS_SIZE + 1);
    struct RuleSelector* selector;
    css_iter(selector, selectors) {
        struct Rule** nested_rules = unpack_rules(selector, attrs);
        struct Rule* nested_rule;
        css_iter(nested_rule, nested_rules) { // concat rules
            assert(rule_iter <= RULE_SELECTORS_SIZE);
            rules[rule_iter++] = nested_rule;
        }
        // free(nested_rules);
    }

    while(rule_iter <= RULE_SELECTORS_SIZE) {
        rules[rule_iter++] = NULL;
    }

    /*
    struct RuleAttr *attr;
    css_iter(attr, attrs) { // cleaning
        switch (attr->type) {
            case RULE_ATTR_PROP: free(attr->obj); break;
            default: break;
        }
        free(attr);
    }
    free(attrs);
    */

    return rules;
}

struct Prop* make_prop(char *name, struct Obj **objs) {
    struct Prop *prop = MEMORY_ALLOCATE(global_memory, struct Prop);
    prop->name = name;
    prop->objs = objs;
    return prop;
}

struct RuleSelector* make_rule_selector(char* element, char** klasses) {
    struct RuleSelector *rule_selector = MEMORY_ALLOCATE(global_memory, struct RuleSelector);
    rule_selector->element = element;
    rule_selector->klasses = klasses;
    rule_selector->pseudo_klass = NULL;
    rule_selector->parent = NULL;
    rule_selector->greedy_parent = NULL;
    return rule_selector;
}

struct Obj* make_obj_as_number(int value) {
    struct Obj* obj = MEMORY_ALLOCATE(global_memory, struct Obj);
    int *ptr = MEMORY_ALLOCATE(global_memory, int);
    *ptr = value;
    obj->type = OBJ_NUMBER;
    obj->value = (void*)ptr;
    return obj;
}

struct Obj* make_obj_as_percent(int value) {
    struct Obj* obj = MEMORY_ALLOCATE(global_memory, struct Obj);
    int *ptr = MEMORY_ALLOCATE(global_memory, int);
    *ptr = value;
    obj->type = OBJ_PERCENT;
    obj->value = (void*)ptr;
    return obj;
}

struct Obj* make_obj_as_string(char* string) {
    struct Obj* obj = MEMORY_ALLOCATE(global_memory, struct Obj);
    obj->type = OBJ_STRING;
    obj->value = (void*)string;
    return obj;
}

struct Obj* make_obj_as_variable(char* variable) {
    struct Obj* obj = MEMORY_ALLOCATE(global_memory, struct Obj);
    obj->type = OBJ_VARIABLE;
    obj->value = (void*)variable;
    return obj;
}

struct Obj* make_obj_as_color(struct rgb* color) {
    struct Obj* obj = MEMORY_ALLOCATE(global_memory, struct Obj);
    obj->type = OBJ_COLOR;
    obj->value = (void*)color;
    return obj;
}

struct Obj* make_obj_as_rule(struct RuleSelector* rule_selector) {
    struct Obj* obj = MEMORY_ALLOCATE(global_memory, struct Obj);
    obj->type = OBJ_RULE;
    obj->value = (void*)rule_selector;
    return obj;
}

struct Obj* _make_obj_as_op(enum ObjType type, struct Obj* left, struct Obj* right) {
    struct PairObj* pair = MEMORY_ALLOCATE(global_memory, struct PairObj);
    pair->left = left;
    pair->right = right;

    struct Obj* obj = MEMORY_ALLOCATE(global_memory, struct Obj);
    obj->type = type;
    obj->value = (void*)pair;

    return obj;
}

#define APPEND_OP(name, op) \
    struct Obj* make_obj_as_ ## name(struct Obj* left, struct Obj* right) {\
        return _make_obj_as_op(op, left, right);\
    }

APPEND_OP(add, OBJ_ADD)
APPEND_OP(sub, OBJ_SUB)
APPEND_OP(mul, OBJ_MUL)
APPEND_OP(div, OBJ_DIV)

struct Obj* make_obj_as_func(char* name, struct Obj** args) {
    size_t size = 0;
    while(args[size]) size++; // counter

    struct FuncObj* func = MEMORY_ALLOCATE(global_memory, struct FuncObj);
    func->name = name;
    func->args = args;
    func->args_size = size;

    struct Obj* obj = MEMORY_ALLOCATE(global_memory, struct Obj);
    obj->type = OBJ_FUNC;
    obj->value = (void*)func;
    return obj;
}

struct Obj* make_obj_as_noargs_func(char* name) {
    struct Obj** args = MEMORY_ALLOCATE(global_memory, struct Obj*);
    args[0] = NULL;
    return make_obj_as_func(name, args);
}

%}

%start program
%union {
    char sIndex; // symbol table index
    int number;
    char* string;
    struct rgb* color;
    struct Program* programPtr;
    struct Rule* rulePtr;
    struct Prop* propPtr;
    struct RuleAttr* ruleAttrPtr;
    struct Obj* objPtr;
    struct RuleSelector* ruleSelectorPtr;
    struct RuleSelector** ruleSelectorPtrMany;
    struct Rule** rulePtrMany;
    struct RuleAttr** ruleAttrPtrMany;
    struct Obj** objPtrMany;
    char** strMany;
};

%token
    START_BODY END_BODY START_FUNC END_FUNC
    COLON SEMICOLON COMMA PERCENT
    ADD_OP SUB_OP MUL_OP DIV_OP PARENT_OP PARENT_SELECTOR_OP
    SPACE
%token <string> WORD STRING CLASS PSEUDO_CLASS VARIABLE
%token <number> NUMBER
%token <color> COLOR
%left ADD_OP SUB_OP
%left MUL_OP DIV_OP
%right START_FUNC END_FUNC
%right START_BODY END_BODY
%type <programPtr> program
%type <rulePtrMany> rule
%type <string> rule_selector_word
%type <propPtr> prop
%type <ruleAttrPtr> rule_or_prop
%type <objPtr> obj
%type <rulePtrMany> rules
%type <ruleAttrPtrMany> rules_and_props
%type <objPtrMany> objs args
%type <ruleSelectorPtr> rule_selector rule_selector_in_rule rule_selector_simple
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
        rule_selectors START_BODY sp rules_and_props END_BODY sp { $$ = make_rules($1, $4); }
        ;

rule_selectors:
        rule_selector_in_rule { make_array(struct RuleSelector, RULE_SELECTORS_SIZE, $1); $$ = arr; }
        | rule_selectors COMMA sp rule_selector_in_rule { append_to_array($1, RULE_SELECTORS_SIZE, $4); $$ = $1; }
        ;

classes:
        CLASS { make_array(char, KLASSES_SIZE, $1); $$ = arr; }
        | classes CLASS { append_to_array($$, KLASSES_SIZE, $2); }
        ;

rule_selector_in_rule:
        rule_selector { $$ = $1; }
        | rule_selector_in_rule PARENT_OP sp rule_selector { $4->parent = $1; $$ = $4; }
        | rule_selector_in_rule rule_selector { $2->greedy_parent = $1; $$ = $2; }
        ;

rule_selector:
        rule_selector_word { $$ = make_rule_selector($1, NULL); }
        | rule_selector_word classes sp { $$ = make_rule_selector($1, $2); }
        | classes sp { $$ = make_rule_selector(NULL, $1); }
        ; /* TODO - pseudoklasses */

rule_selector_word:
        WORD sp { $$ = $1; }
        | PARENT_SELECTOR_OP sp { $$ = MEMORY_ALLOCATE_ARRAY(global_memory, char, 2); memcpy($$, "&", 2); }
        | MUL_OP sp { $$ = MEMORY_ALLOCATE_ARRAY(global_memory, char, 2); memcpy($$, "*", 2); }
        ;

rules_and_props:
        rule_or_prop { make_array(struct RuleAttr, RULE_ATTRS_SIZE, $1); $$ = arr; }
        | rules_and_props rule_or_prop { append_to_array($1, RULE_ATTRS_SIZE, $2); $$ = $1; }
        ;

rule_or_prop:
        prop { $$ = make_rule_attr($1, RULE_ATTR_PROP); }
        | rule { $$ = make_rule_attr($1, RULE_ATTR_RULE); }
        ;

prop:
        WORD sp COLON sp objs SEMICOLON sp{ $$ = make_prop($1, $5); }
        ;

objs:
        obj { make_array(struct Obj, OBJS_SIZE, $1); $$ = arr; }
        | objs COMMA sp obj { append_to_array($1, OBJS_SIZE, $4); $$ = $1; }
        ;

obj:
        NUMBER sp { $$ = make_obj_as_number($1); }
        | NUMBER PERCENT sp { $$ = make_obj_as_percent($1); }
        | STRING sp { $$ = make_obj_as_string($1); }
        | VARIABLE sp { $$ = make_obj_as_variable($1); }
        | COLOR sp { $$ = make_obj_as_color($1); }
        | rule_selector_simple { $$ = make_obj_as_rule($1); }
        | obj ADD_OP sp obj { $$ = make_obj_as_add($1, $4); }
        | obj SUB_OP sp obj { $$ = make_obj_as_sub($1, $4); }
        | obj MUL_OP sp obj { $$ = make_obj_as_mul($1, $4); }
        | obj DIV_OP sp obj { $$ = make_obj_as_div($1, $4); }
        | START_FUNC sp obj END_FUNC sp { $$ = $3; }
        | WORD sp START_FUNC sp END_FUNC sp { $$ = make_obj_as_noargs_func($1); }
        | WORD sp START_FUNC sp args END_FUNC sp { $$ = make_obj_as_func($1, $5); }
        ;

rule_selector_simple:
        WORD sp { $$ = make_rule_selector($1, NULL); }
        | WORD classes sp { $$ = make_rule_selector($1, $2); }
        | classes sp { $$ = make_rule_selector(NULL, $1); }
        ;

args:
        obj { make_array(struct Obj, OBJS_SIZE, $1); $$ = arr; }
        | args COMMA sp obj { append_to_array($1, OBJS_SIZE, $4); $$ = $1; }
        ;

sp: SPACE | sp SPACE | /* empty */;
%%

struct Program* css_parse_file(char* filename) {
    FILE *stream = fopen(filename, "r");
    struct Program* program = css_parse_file_as_stream(stream);
    fclose(stream);
    return program;
}

struct Program* css_parse_file_as_stream(FILE* stream) {
    lines = 1;
    old_chars = 1;
    chars = 1;
    global_memory = memory_create(1024 * 1024);
    yyin = stream;
    yyparse();
    return global_program;
}
