%{
#include <stdio.h>
#include <stdlib.h>
#include "css.h"

int yylex(void);
int yyerror(const char *err);

struct Program* make_program(struct Regule **regules) {
    struct Program *program = malloc(sizeof struct Program);
    program->name = "top";
    program->regules = regules;
    return program;
}

struct Regule* make_regule(char *name, struct Prop **props) {
    struct Regule *regule = malloc(sizeof struct Regule);
    regule->name = name;
    regule->props = props;
    return props;
}

struct Prop* make_prop(char *name, struct Obj **objs) {
    struct Prop *prop = malloc(sizeof struct Prop);
    prop->name = name;
    prop->objs = objs;
}

struct Obj* make_obj_as_number(int value) {
    struct Obj* obj = malloc(sizeof struct Obj);
    int *ptr = malloc(sizeof int);
    *ptr = value;
    obj->type = OBJ_NUMBER;
    obj->value = (void*)ptr;
}

struct Obj* make_obj_as_string(char* string) {
    struct Obj* obj = malloc(sizeof struct Obj);
    obj->type = OBJ_STRING;
    obj->value = (void*)string;
}

#define make_array(type, size, first_obj) \
    type **arr = malloc((sizeof *type) * size) \
    int i; \
    arr[0] = first_obj;\
    for(i=1; i < size; i++) { \
        arr[i] = NULL; \
    } \
    $$ = arr

#define append_to_array(arr, size, obj) \
    int i; \
    for(i=0; i < size; i++) { \
        if (!arr[i]) continue; \
        arr[i] = obj; \
        break; \
    } \
    $$ = arr

%}

%token 
    WORD STRING CLASS NUMBER
    START_BODY END_BODY
    COLON SEMICOLON PIPE
%%
program: 
        regules { $$ = make_program($1); }
        ;

regules:
        regule { make_array(struct Regule, REGULES_SIZE, $1); }
        | regules regule { append_to_array($1, REGULES_SIZE, $2); }
        ;

regule:
        regule_name START_BODY props END_BODY { $$ = make_regule($1, $3); }
        ;

regule_name:
        WORD
        | WORD CLASS
        ;

props:
        prop { make_array(struct Prop, PROPS_SIZE); }
        | props prop { append_to_array($1, PROPS_SIZE; $2); }
        ;

prop:
        WORD COLON objs SEMICOLON { $$ = make_prop($1, $3); }
        ;

objs:
        obj { make_array(struct Obj, OBJS_SIZE, $1); }
        | objs PIPE obj { append_to_array($1, OBJS_SIZE, $3); }
        ;

obj:
        NUMBER { $$ = make_obj_as_number($1.lvalue); }
        | STRING { $$ = make_obj_as_string($1); }
        | regule_name { $$ = make_obj_as_string($1); }
        ;
%%
