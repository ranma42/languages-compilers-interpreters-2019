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
%token LIT_TRUE
%token LIT_FALSE
%type <e> expr

%left '&' '|'
%nonassoc '<' '>' LE GE '=' NE
%left '+' '-'
%left '*' '/'
%nonassoc '!'

%%

program: program expr '\n' {
    switch (check_types($2)) {
      case ERROR:
        printf("=> type checking failed\n");
        break;
      case INTEGER:
        printf("=> int\n");
        break;
      case BOOLEAN:
        printf("=> bool\n");
        break;
    }
    
    print_expr_stack($2);
    free_expr($2);
    }
  |
  ;

expr: VAL { $$ = make_val($1); }
      | LIT_TRUE { $$ = make_bool(1); }
      | LIT_FALSE { $$ = make_bool(0); }
      | IDENTIFIER { $$ = make_identifier($1); }

      | '!' expr      { $$ = make_un_op('!', $2); }

      | expr '+' expr { $$ = make_bin_op($1, '+', $3); }
      | expr '*' expr { $$ = make_bin_op($1, '*', $3); }
      | expr '-' expr { $$ = make_bin_op($1, '-', $3); }
      | expr '/' expr { $$ = make_bin_op($1, '/', $3); }

      | expr '<' expr { $$ = make_bin_op($1, '<', $3); }
      | expr '>' expr { $$ = make_bin_op($1, '>', $3); }
      | expr LE  expr { $$ = make_bin_op($1, LE, $3); }
      | expr GE  expr { $$ = make_bin_op($1, GE, $3); }

      | expr '=' expr { $$ = make_bin_op($1, '=', $3); }
      | expr NE  expr { $$ = make_bin_op($1, NE, $3); }

      | expr '&' expr { $$ = make_bin_op($1, '&', $3); }
      | expr '|' expr { $$ = make_bin_op($1, '|', $3); }

      | '(' expr ')'  { $$ = $2; }

%%

int main(void) {
  yyparse();
  return 0;
}
