        int row = 1, col = 1;

COMMA         ,
CR            \r
LF            \n
TEXTDATA      [^,"\0-\x1F]
DQUOTE        \"
DDQUOTE       \"\"

%x escaped

%%

{CR}{LF}            printf("\n=== NEW RECORD ===\n"); col = 1; row++;
{COMMA}             printf("\n"); col++;
{TEXTDATA}*         printf("row %d col %d: >%s<", row, col, yytext);

{DQUOTE}            BEGIN(escaped); printf("row %d col %d: >", row, col);
<escaped>{DQUOTE}   BEGIN(INITIAL); printf("<");
<escaped>{DDQUOTE}  printf("\"");
<escaped>({TEXTDATA}|{COMMA}|{CR}|{LF})*  printf("%s", yytext);

.                   printf("\nSYNTAX ERROR\n");

%%

int yywrap() {
  printf("\n=== DONE ===\n");
  return 1;
}

int main() {
  yylex();
  return 0;
}
