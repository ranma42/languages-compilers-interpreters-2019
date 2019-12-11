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
%token LET_KW IN_KW VAR_KW
%token IF_KW THEN_KW ELSE_KW
%token WHILE_KW DO_KW
%token LIT_TRUE LIT_FALSE
%type <e> expr

%right DO_KW
%right ELSE_KW
%right IN_KW
%nonassoc ASSIGN_OP
%left '&' '|'
%nonassoc '<' '>' LE GE '=' NE
%left '+' '-'
%left '*' '/'
%nonassoc '!'

%%

program: program expr '\n' {
    jit_eval($2);
    free_expr($2);
    }
  |
  ;

expr: VAL { $$ = make_val($1); }
      | LIT_TRUE { $$ = make_bool(1); }
      | LIT_FALSE { $$ = make_bool(0); }
      | IDENTIFIER { $$ = make_identifier($1); }

      | LET_KW IDENTIFIER '=' expr IN_KW expr
                      { $$ = make_let($2, $4, $6); }

      | VAR_KW IDENTIFIER '=' expr IN_KW expr
                      { $$ = make_var($2, $4, $6); }

      | IDENTIFIER ASSIGN_OP expr
                      { $$ = make_assign($1, $3); }

      | IDENTIFIER '(' expr ')'
                      { $$ = make_call($1, $3); }

      | IF_KW expr THEN_KW expr ELSE_KW expr
                      { $$ = make_if($2, $4, $6); }

      | WHILE_KW expr DO_KW expr
                      { $$ = make_while($2, $4); }

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
      | '\n' expr { $$ = $2; }

%%

int main(void) {
  yyparse();

  return 0;
}
