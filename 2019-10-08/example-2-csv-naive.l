        int row = 1, col = 1;

%%

\r\n          printf("\n=== NEW RECORD ===\n"); col = 1; row++;
,             printf("\n"); col++;
[^,\0-\x1F]+  printf("row %d col %d: >%s<", row, col, yytext);

.             printf("\nSYNTAX ERROR\n");

%%

int yywrap() {
  printf("\n=== DONE ===\n");
  return 1;
}

int main() {
  yylex();
  return 0;
}
