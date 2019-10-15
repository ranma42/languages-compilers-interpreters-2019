%%

\a       printf("\\a");
\b       printf("\\b");
\f       printf("\\f");
\n       printf("\\n");
\r       printf("\\r");
\t       printf("\\t");
\\       printf("\\\\");
\"       printf("\\\"");
\'       printf("\\\'");
\?       printf("\\\?");
[ -~]    printf("%s", yytext);
.        printf("\\%03o", yytext[0] & 0xFF);

%%

int yywrap(void)
{
  printf("\"\n");
  return 1;
}

int main(void) {
  printf("\"");
  yylex();
  return 0;
}
