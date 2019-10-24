%{
  #include <stdio.h>

  #include "ast.h"

  int yylex(void);
  void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
  }
%}

%union {
  int lit_value;
  char *ident;
  struct expr* e;
}

%token <ident> IDENTIFIER
%token <lit_value> VAL
%type <e> expr

%left '+' '-'
%left '*' '/'

%%

program: program expr '\n' { print_expr($2); free_expr($2); }
  |
  ;

expr: VAL { $$ = make_val($1); }
      | IDENTIFIER { $$ = make_identifier($1); }
      | expr '+' expr { $$ = make_bin_op($1, '+', $3); }
      | expr '*' expr { $$ = make_bin_op($1, '*', $3); }
      | expr '-' expr { $$ = make_bin_op($1, '-', $3); }
      | expr '/' expr { $$ = make_bin_op($1, '/', $3); }
      | '(' expr ')'  { $$ = $2; }

%%

int main(void) {
  yyparse();
  return 0;
}
