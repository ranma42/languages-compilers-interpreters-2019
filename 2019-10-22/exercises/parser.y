%{
  #include <stdio.h>
  int yylex(void);
  void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
  }

  int reg_index = 0;

  int gen_reg() {
    return reg_index++;
  }
%}

%token VAL

%left '+' '-'
%left '*' '/'

%%

program: program expr '\n' { printf("-> r%d\n", $2); }
  |
  ;

expr: VAL { $$ = gen_reg(); printf("load r%d, %d\n", $$, $1); }
      | expr '+' expr { $$ = gen_reg(); printf("add r%d, r%d, r%d\n", $$, $1, $3); }
      | expr '*' expr { $$ = gen_reg(); printf("mul r%d, r%d, r%d\n", $$, $1, $3); }
      | expr '-' expr { $$ = gen_reg(); printf("sub r%d, r%d, r%d\n", $$, $1, $3); }
      | expr '/' expr { $$ = gen_reg(); printf("div r%d, r%d, r%d\n", $$, $1, $3); }
      | '(' expr ')'  { $$ = $2; }

%%

int main(void) {
  yyparse();
  return 0;
}
