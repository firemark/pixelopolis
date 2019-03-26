%{
#include <stdio.h>
#include <string.h>

#include "css.y.h"

#define push_string \
    out = malloc(sizeof(char*) * (yyleng + 1)); \
    strncpy(out, yytext, yyleng); \
    out[yyleng] = 0; \
    yylval.string = out

#define crop_and_push_string(l, r) \
    out = malloc(sizeof(char*) * (yyleng + 1 - r)); \
    strncpy(out, yytext + l, yyleng - r); \
    out[yyleng - r] = 0; \
    yylval.string = out

#define push_number yylval.number = atoi(yytext)

char* out;
int lines = 1;
int chars = 1;

%}

%%
.                           chars++; REJECT;
[a-zA-Z_][a-zA-Z0-9_-]*     push_string; return WORD;
\"[^"]+\"                   crop_and_push_string(1, 2); return STRING;
\.[a-zA-Z_][a-zA-Z0-9_-]*   crop_and_push_string(1, 0); return CLASS;
\$[a-zA-Z0-9_-]*            crop_and_push_string(1, 0); return VARIABLE;
:[a-zA-Z_][a-zA-Z0-9_-]*    crop_and_push_string(1, 0); return PSEUDO_CLASS;
[0-9]+                      push_number; return NUMBER;
\{                          return START_BODY;
\}                          return END_BODY;
\(                          return START_FUNC;
\)                          return END_FUNC;
:                           return COLON;
;                           return SEMICOLON;
,                           return COMMA;
\|                          return PIPE;
\*                          return MUL_OP;
\/                          return DIV_OP;
\+                          return ADD_OP;
-                           return SUB_OP;
\n                          chars = 1; lines++;
[ \r\t]+                    /* ignore */
%%
