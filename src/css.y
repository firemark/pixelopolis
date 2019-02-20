%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
int yyerror(const char *err);

%}

%token 
    WORD STRING CLASS NUMBER
    START_BODY END_BODY
    COLON SEMICOLON PIPE
%%
program: 
        regules
        ;

regules:
        regule
        | regules regule
        ;

regule:
        regule_name START_BODY props END_BODY
        ;

regule_name:
        WORD
        | WORD CLASS 
        ;

props:
        prop
        | props prop
        ;

prop:
        WORD COLON objs SEMICOLON
        ;

objs:
        obj
        | objs PIPE obj
        ;

obj:
        NUMBER
        | STRING
        | regule_name
        ;
%%
