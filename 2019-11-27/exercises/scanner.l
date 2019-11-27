%{
  #include "y.tab.h"
%}

%option noyywrap
%%

true                   return LIT_TRUE;
false                  return LIT_FALSE;
[A-Za-z_][A-Za-z_0-9]* { yylval.ident = strdup(yytext); return IDENTIFIER; }
[0-9]+                 { yylval.lit_value = atoi(yytext); return VAL; }
[-+*/\n()<>=&|!]       return *yytext;
\<=                    return LE;
\>=                    return GE;
!=                     return NE;
[[:space:]]            /* ignore */
.                      return *yytext;

%%
