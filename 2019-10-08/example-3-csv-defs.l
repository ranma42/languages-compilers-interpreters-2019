        int row = 1, col = 1;

COMMA         ,
CR            \r
LF            \n
TEXTDATA      [^,\0-\x1F]

%x escaped

%%

{CR}{LF}            printf("\n=== NEW RECORD ===\n"); col = 1; row++;
{COMMA}             printf("\n"); col++;
{TEXTDATA}+         printf("row %d col %d: >%s<", row, col, yytext);

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
