%{
#include <stdio.h>
#include <string.h>

#include "css.y.h"

#define push_string \
    out = malloc(sizeof(char*) * (yyleng + 1)); \
    strncpy(out, yytext, yyleng); \
    out[yyleng] = 0; \
    yylval.string = out

#define push_number yylval.number = atoi(yytext)

char* out;
int lines = 1;
int chars = 1;

%}

%%
.                           chars++; REJECT;
[a-zA-Z_][a-zA-Z0-9_]*      push_string; return WORD;
\"[^"]+\"                   push_string; return STRING;
\.[a-zA-Z_][a-zA-Z0-9_]*    push_string; return CLASS;
[0-9]+                      push_number; return NUMBER;
\{                          return START_BODY;
\}                          return END_BODY;
:                           return COLON;
;                           return SEMICOLON;
\|                          return PIPE;
\n                          chars = 1; lines++;
[ \r\t]+                    /* ignore */
%%
