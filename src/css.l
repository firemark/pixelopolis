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
#define count_chars old_chars = chars; chars += yyleng

char* out;
int lines = 1;
int old_chars = 1;
int chars = 1;

%}

%%
[a-zA-Z_][a-zA-Z0-9_-]*     count_chars; push_string; return WORD;
\"[^"]+\"                   count_chars; crop_and_push_string(1, 2); return STRING;
\.[a-zA-Z_][a-zA-Z0-9_-]*   count_chars; crop_and_push_string(1, 0); return CLASS;
:[a-zA-Z_][a-zA-Z0-9_-]*    count_chars; crop_and_push_string(1, 0); return PSEUDO_CLASS;
[0-9]+                      count_chars; push_number; return NUMBER;
\{                          count_chars; return START_BODY;
\}                          count_chars; return END_BODY;
\(                          count_chars; return START_FUNC;
\)                          count_chars; return END_FUNC;
:                           count_chars; return COLON;
;                           count_chars; return SEMICOLON;
,                           count_chars; return COMMA;
%                           count_chars; return PERCENT;
\|                          count_chars; return PIPE;
\*                          count_chars; return MUL_OP;
\/                          count_chars; return DIV_OP;
\+                          count_chars; return ADD_OP;
-                           count_chars; return SUB_OP;
>                           count_chars; return PARENT_OP;
\n                          old_chars = 1; chars = 1; lines++;
[ \r\t]+                    count_chars; return SPACE;
%%
