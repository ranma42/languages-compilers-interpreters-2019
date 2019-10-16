%{
  #include <stdio.h>
  int yylex(void);
  void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
  }
%}

%token VAL

%left '+' '-'
%left '*' '/'

%%

program: program expr { printf("%d\n", $2); }
  |
  ;

expr: VAL { $$ = $1; printf("load %d # %d\n", $1, $1); }
      | expr '+' expr { $$ = $1 + $3; printf("add # %d + %d\n", $1, $3); }
      | expr '*' expr { $$ = $1 * $3; printf("mul # %d * %d\n", $1, $3); }
      | expr '-' expr { $$ = $1 - $3; printf("sub # %d - %d\n", $1, $3); }
      | expr '/' expr { $$ = $1 / $3; printf("div # %d / %d\n", $1, $3); }

%%

int main(void) {
  yyparse();
  return 0;
}
