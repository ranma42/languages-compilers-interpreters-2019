SINT       [-+]?[0-9]+
EXP        [eE]{SINT}

%%

{SINT}{EXP}?                printf("%s -> %g\n", yytext, atof(yytext));
{SINT}\.[0-9]*{EXP}?        printf("%s -> %g\n", yytext, atof(yytext));
[-+]?[0-9]*\.[0-9]+{EXP}?   printf("%s -> %g\n", yytext, atof(yytext));
[ \t\n\r]                   /* ignore whitespace */
.                           printf("WARNING: unexpected symbol '%s'\n", yytext);

%%

int yywrap(void)
{
  return 1;
}

int main(void) {
  yylex();
  return 0;
}
