%{
  #include <stdio.h>

  #include "ast.h"

  int yylex(void);
  void yyerror(LLVMModuleRef module, LLVMBuilderRef builder, char *s) {
    fprintf(stderr, "%s\n", s);
  }
%}

%parse-param {LLVMModuleRef module}
%parse-param {LLVMBuilderRef builder}

%union {
  int lit_value;
  char *ident;
  struct expr* e;
}

%token <ident> IDENTIFIER
%token <lit_value> VAL
%token LET_KW IN_KW
%token LIT_TRUE LIT_FALSE
%type <e> expr

%right IN_KW
%left '&' '|'
%nonassoc '<' '>' LE GE '=' NE
%left '+' '-'
%left '*' '/'
%nonassoc '!'

%%

program: program expr '\n' {
    // LLVM can only emit instructions in basic blocks
    //   basic blocks are always part of a function
    //   function are contained in modules

    // create function to wrap expr
    LLVMTypeRef f_type = LLVMFunctionType(LLVMVoidType(), NULL, 0, 0);
    LLVMValueRef f = LLVMAddFunction(module, "f", f_type);
    LLVMBasicBlockRef entry_bb = LLVMAppendBasicBlock(f, "entry");
    LLVMPositionBuilderAtEnd(builder, entry_bb);

    // emit expression as function body and ignore the result
    codegen_expr($2, NULL, module, builder);

    // terminate the basic block
    LLVMBuildRet(builder, 0);

    LLVMDumpValue(f);

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
  LLVMModuleRef module = LLVMModuleCreateWithName("exe");
  LLVMBuilderRef builder = LLVMCreateBuilder();

  yyparse(module, builder);

  LLVMDisposeBuilder(builder);
  LLVMDisposeModule(module);

  return 0;
}
