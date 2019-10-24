%{
  #include "y.tab.h"
%}

%option noyywrap
%%

[0-9]+        { yylval = atoi(yytext); return VAL; }
[-+*/\n()]      return *yytext;
[[:space:]]   /* ignore */
.             return *yytext;

%%
