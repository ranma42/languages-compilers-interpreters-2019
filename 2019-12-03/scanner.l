%{
  #include <llvm-c/Core.h>
  #include "y.tab.h"
%}

%option noyywrap
%%
if                     return IF_KW;
then                   return THEN_KW;
else                   return ELSE_KW;
while                  return WHILE_KW;
do                     return DO_KW;
var                    return VAR_KW;
let                    return LET_KW;
in                     return IN_KW;
true                   return LIT_TRUE;
false                  return LIT_FALSE;
[A-Za-z_][A-Za-z_0-9]* { yylval.ident = strdup(yytext); return IDENTIFIER; }
[0-9]+                 { yylval.lit_value = atoi(yytext); return VAL; }
[-+*/\n()<>=&|!]       return *yytext;
\<=                    return LE;
\>=                    return GE;
!=                     return NE;
:=                     return ASSIGN_OP;
[[:space:]]            /* ignore */
.                      return *yytext;

%%
