%{
#include <string.h>

#include "css.y.h"
#include "basic.h"

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

#define push_hex \
    color = malloc(sizeof(struct rgb)); \
    color->r = two_chars_to_hex(&yytext[1]); \
    color->g = two_chars_to_hex(&yytext[3]); \
    color->b = two_chars_to_hex(&yytext[5]); \
    yylval.color = color;

#define push_small_hex \
    color = malloc(sizeof(struct rgb)); \
    color->r = char_to_hex_double(yytext[1]); \
    color->g = char_to_hex_double(yytext[2]); \
    color->b = char_to_hex_double(yytext[3]); \
    yylval.color = color;

struct rgb* color;
char* out;
int lines = 1;
int old_chars = 1;
int chars = 1;

unsigned char char_to_hex(char x) {
    if (x >= 'A' && x <= 'Z') return x - 'A' + 10;
    if (x >= 'a' && x <= 'z') return x - 'a' + 10;
    if (x >= '0' && x <= '9') return x - '0';
    return 0;
}

unsigned char two_chars_to_hex(char* x) {
    unsigned char a = char_to_hex(x[0]);
    unsigned char b = char_to_hex(x[1]);
    return a << 4 | b;
}

unsigned char char_to_hex_double(char x) {
    unsigned char result = char_to_hex(x);
    return result << 4 | result;
}

%}

%%
\/\*.+\*\/                  count_chars;
[a-zA-Z_][a-zA-Z0-9_-]*     count_chars; push_string; return WORD;
\"[^"]+\"                   count_chars; crop_and_push_string(1, 2); return STRING;
\.[a-zA-Z_][a-zA-Z0-9_-]*   count_chars; crop_and_push_string(1, 0); return CLASS;
:[a-zA-Z_][a-zA-Z0-9_-]*    count_chars; crop_and_push_string(1, 0); return PSEUDO_CLASS;
[0-9]+                      count_chars; push_number; return NUMBER;
\#[0-9A-Fa-f]{6}            count_chars; push_hex; return COLOR;
\#[0-9A-Fa-f]{3}            count_chars; push_small_hex; return COLOR;
\{                          count_chars; return START_BODY;
\}                          count_chars; return END_BODY;
\(                          count_chars; return START_FUNC;
\)                          count_chars; return END_FUNC;
:                           count_chars; return COLON;
;                           count_chars; return SEMICOLON;
,                           count_chars; return COMMA;
%                           count_chars; return PERCENT;
\*                          count_chars; return MUL_OP;
\/                          count_chars; return DIV_OP;
\+                          count_chars; return ADD_OP;
-                           count_chars; return SUB_OP;
>                           count_chars; return PARENT_OP;
\n                          old_chars = 1; chars = 1; lines++;
[ \r\t]+                    count_chars; return SPACE;
%%
