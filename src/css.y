%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "css.h"

FILE *yyin;
struct Program* global_program;
int lines;
int chars;
int yylex(void);

int yyerror (char* err) {
    fprintf(stderr, "%d:%d %s\n", lines, chars, err);
}

int yywrap (void) {
    return 1;
}

struct Program* make_program(struct Rule **rules) {
    struct Program *program = malloc(sizeof(struct Program));
    program->name = "top";
    program->rules = rules;
    return program;
}

struct Rule* make_rule(char *name, struct Prop **props) {
    struct Rule *rule = malloc(sizeof(struct Rule));
    rule->name = name;
    rule->props = props;
    return rule;
}

struct Prop* make_prop(char *name, struct Obj **objs) {
    struct Prop *prop = malloc(sizeof(struct Prop));
    prop->name = name;
    prop->objs = objs;
    return prop;
}

struct Obj* make_obj_as_number(int value) {
    struct Obj* obj = malloc(sizeof(struct Obj));
    int *ptr = malloc(sizeof(int));
    *ptr = value;
    obj->type = OBJ_NUMBER;
    obj->value = (void*)ptr;
}

struct Obj* make_obj_as_string(char* string) {
    struct Obj* obj = malloc(sizeof(struct Obj));
    obj->type = OBJ_STRING;
    obj->value = (void*)string;
}

#define make_array(type, size, first_obj) \
    type **arr = malloc(sizeof(type*) * (size + 1)); \
    size_t i; \
    arr[0] = first_obj; \
    for(i=1; i < size; i++) { \
        arr[i] = NULL; \
    } \

#define append_to_array(arr, size, obj) \
    size_t i; \
    for(i=0; i < size; i++) { \
        if (arr[i]) continue; \
        arr[i] = obj; \
        break; \
    } \

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
    struct Rule** rulePtrMany;
    struct Prop** propPtrMany;
    struct Obj** objPtrMany;
};

%token 
    START_BODY END_BODY
    COLON SEMICOLON PIPE
%token <string> WORD STRING CLASS
%token <number> NUMBER
%type <string> rule_name
%type <programPtr> program
%type <rulePtr> rule
%type <propPtr> prop
%type <objPtr> obj
%type <rulePtrMany> rules;
%type <propPtrMany> props;
%type <objPtrMany> objs;

%%
program: 
        rules { global_program = make_program($1); }
        ;

rules:
        rule { make_array(struct Rule, REGULES_SIZE, $1); $$ = arr; }
        | rules rule { append_to_array($1, REGULES_SIZE, $2); $$ = $1; }
        ;

rule:
        rule_name START_BODY props END_BODY { $$ = make_rule($1, $3); }
        ;

rule_name:
        WORD
        | WORD CLASS {
            size_t size = strlen($1) + strlen($2);
            char* out = malloc(sizeof(char) * (size + 1));
            strcpy(out, $1);
            strcat(out, $2);
            free($1);
            free($2);
            $$ = out;
        }
        ;

props:
        prop { make_array(struct Prop, PROPS_SIZE, $1); $$ = arr; }
        | props prop { append_to_array($1, PROPS_SIZE, $2); $$ = $1; }
        ;

prop:
        WORD COLON objs SEMICOLON { $$ = make_prop($1, $3); }
        ;

objs:
        obj { make_array(struct Obj, OBJS_SIZE, $1); $$ = arr; }
        | objs PIPE obj { append_to_array($1, OBJS_SIZE, $3); $$ = $1; }
        ;

obj:
        NUMBER { $$ = make_obj_as_number($1); }
        | STRING { $$ = make_obj_as_string($1); }
        | rule_name { $$ = make_obj_as_string($1); }
        ;

%%

struct Program* css_parse_file(char* filename) {
    yyin = fopen(filename, "r");
    yyparse();
    fclose(yyin);
    return global_program;
}
