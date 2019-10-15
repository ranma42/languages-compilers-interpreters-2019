

%x quoted

%%

\"                BEGIN(quoted);
[[:space:]]       /* ignore whitespace */
.                 printf("WARNING: unexpected symbol '%s'\n", yytext);

<quoted>\"        BEGIN(INITIAL);
<quoted>[^"\\]+   printf("%s", yytext);
<quoted>\\a       printf("\a");
<quoted>\\b       printf("\b");
<quoted>\\f       printf("\f");
<quoted>\\n       printf("\n");
<quoted>\\r       printf("\r");
<quoted>\\t       printf("\t");
<quoted>\\\\      printf("\\");
<quoted>\\\"      printf("\"");
<quoted>\\\'      printf("\'");
<quoted>\\\?      printf("\?");
<quoted>\\x[0-9a-fA-F]+   printf("%c", (int) strtol(yytext+2, NULL, 16));
<quoted>\\[0-7]{1,3}      printf("%c", (int) strtol(yytext+1, NULL, 8));

%%

int yywrap(void)
{
  return 1;
}

int main(void) {
  yylex();
  return 0;
}
