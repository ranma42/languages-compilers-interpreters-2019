        int num_lines = 0, num_chars = 0;

%%

.       num_chars++;
\n      num_lines++; num_chars++;

%%

int yywrap() {
  printf("%d lines, %d chars\n", num_lines, num_chars);
  return 1;
}

int main() {
  yylex();
  return 0;
}
