%{
#include <stdio.h>
#include <string.h>
#include "y.tab.h"
%}

%%
[a-zA-Z_][a-zA-Z0-9_]*      return WORD;
\"[^"]+\"                   return STRING;
\.[a-zA-Z_][a-zA-Z0-9_]*    return CLASS;
[0-9]+                      yylval=atoi(yytext); return NUMBER;
{                           return START_BODY;
}                           return END_BODY;   
:                           return COLON;
;                           return SEMICOLON;
\|                          return PIPE;
\n                          /* ignore */
[\s\t]+                     /* ignore */
%%
