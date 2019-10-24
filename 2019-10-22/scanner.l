%{
  #include "y.tab.h"
%}

%option noyywrap
%%

[A-Za-z_][A-Za-z_0-9]* { yylval.ident = strdup(yytext); return IDENTIFIER; }
[0-9]+                 { yylval.lit_value = atoi(yytext); return VAL; }
[-+*/\n()]             return *yytext;
[[:space:]]            /* ignore */
.                      return *yytext;

%%
