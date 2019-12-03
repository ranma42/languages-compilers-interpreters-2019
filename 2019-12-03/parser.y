%{
  #include <stdio.h>
  #include <llvm-c/ExecutionEngine.h>
  #include "ast.h"

  struct llvm_data {
    LLVMModuleRef module;
    LLVMBuilderRef builder;
    LLVMExecutionEngineRef engine;
  };

  int yylex(void);
  void yyerror(struct llvm_data *data, char *s) {
    fprintf(stderr, "%s\n", s);
  }
%}

%parse-param {struct llvm_data *data}

%union {
  int lit_value;
  char *ident;
  struct expr* e;
}

%token <ident> IDENTIFIER
%token <lit_value> VAL
%token LET_KW IN_KW
%token IF_KW THEN_KW ELSE_KW
%token WHILE_KW DO_KW
%token LIT_TRUE LIT_FALSE
%type <e> expr

%right DO_KW
%right ELSE_KW
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
    LLVMValueRef f = LLVMAddFunction(data->module, "f", f_type);
    LLVMBasicBlockRef entry_bb = LLVMAppendBasicBlock(f, "entry");
    LLVMPositionBuilderAtEnd(data->builder, entry_bb);

    // emit expression as function body and ignore the result
    codegen_expr($2, NULL, data->module, data->builder);

    // terminate the basic block
    LLVMBuildRet(data->builder, 0);

    LLVMDumpValue(f);

    fprintf(stderr, "jitting...\n");
    void (*fn)() = LLVMGetPointerToGlobal(data->engine, f);
    fprintf(stderr, "running... %p\n", fn);
    fn();
    fprintf(stderr, "done\n");

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

%%

int main(void) {
  LLVMModuleRef module = LLVMModuleCreateWithName("exe");
  LLVMBuilderRef builder = LLVMCreateBuilder();
  LLVMExecutionEngineRef engine;

  LLVMTypeRef print_i32_args[] = { LLVMInt32Type() };
  LLVMAddFunction(module, "print_i32",
    LLVMFunctionType(LLVMVoidType(), print_i32_args, 1, 0));

  LLVMInitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();
  LLVMInitializeNativeAsmParser();
  LLVMLinkInMCJIT();

  char *error;
  if (LLVMCreateExecutionEngineForModule(&engine, module, &error)) {
    fprintf(stderr, "%s\n", error);
    return 1;
  }

  struct llvm_data data = { module, builder, engine };
  yyparse(&data);

  LLVMDisposeBuilder(builder);
  LLVMDisposeModule(module);

  return 0;
}
