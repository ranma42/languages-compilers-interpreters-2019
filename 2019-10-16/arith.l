%{
  #include "arith.tab.h"
%}

%option noyywrap
%%

[0-9]+        { yylval = atoi(yytext); return VAL; }
[-+*/]      return *yytext;
[[:space:]]   /* ignore */
.             return *yytext;

%%
