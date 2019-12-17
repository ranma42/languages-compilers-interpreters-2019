#include <llvm-c/Analysis.h>
#include <llvm-c/ExecutionEngine.h>
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "y.tab.h"

struct expr *make_val(int value) {
  struct expr *e = malloc(sizeof(struct expr));

  e->type = LITERAL;
  e->value = value;

  return e;
}

struct expr *make_bool(int value) {
  struct expr *e = malloc(sizeof(struct expr));

  e->type = LIT_BOOL;
  e->value = value;

  return e;
}

struct expr *make_identifier(char *ident) {
  struct expr *e = malloc(sizeof(struct expr));

  e->type = IDENT;
  e->ident = ident;

  return e;
}

struct expr *make_call(char *ident, struct expr *expr) {
  struct expr *e = malloc(sizeof(struct expr));

  e->type = CALL;
  e->call.ident = ident;
  e->call.expr = expr;

  return e;
}

struct expr *make_let(char *ident, struct expr *expr, struct expr *body) {
  struct expr *e = malloc(sizeof(struct expr));

  e->type = LET;
  e->let.ident = ident;
  e->let.expr = expr;
  e->let.body = body;

  return e;
}

struct expr *make_var(char *ident, struct expr *expr, struct expr *body) {
  struct expr *e = malloc(sizeof(struct expr));

  e->type = VAR;
  e->var.ident = ident;
  e->var.expr = expr;
  e->var.body = body;

  return e;
}

struct expr *make_assign(char *ident, struct expr *expr) {
  struct expr *e = malloc(sizeof(struct expr));

  e->type = ASSIGN;
  e->assign.ident = ident;
  e->assign.expr = expr;

  return e;
}

struct expr *make_if(struct expr *cond, struct expr *e_true,
                     struct expr *e_false) {
  struct expr *e = malloc(sizeof(struct expr));

  e->type = IF;
  e->if_expr.cond = cond;
  e->if_expr.e_true = e_true;
  e->if_expr.e_false = e_false;

  return e;
}

struct expr *make_while(struct expr *cond, struct expr *body) {
  struct expr *e = malloc(sizeof(struct expr));

  e->type = WHILE;
  e->while_expr.cond = cond;
  e->while_expr.body = body;

  return e;
}

struct expr *make_un_op(int op, struct expr *expr) {
  struct expr *e = malloc(sizeof(struct expr));

  e->type = UN_OP;
  e->unop.op = op;
  e->unop.expr = expr;

  return e;
}

struct expr *make_bin_op(struct expr *lhs, int op, struct expr *rhs) {
  struct expr *e = malloc(sizeof(struct expr));

  e->type = BIN_OP;
  e->binop.lhs = lhs;
  e->binop.op = op;
  e->binop.rhs = rhs;

  return e;
}

void free_expr(struct expr *e) {
  switch (e->type) {
  case LITERAL:
  case LIT_BOOL:
    break;

  case IDENT:
    free(e->ident);
    break;

  case CALL:
    free(e->let.ident);
    free_expr(e->let.expr);
    break;

  case LET:
    free(e->let.ident);
    free_expr(e->let.expr);
    free_expr(e->let.body);
    break;

  case VAR:
    free(e->var.ident);
    free_expr(e->var.expr);
    free_expr(e->var.body);
    break;

  case ASSIGN:
    free(e->assign.ident);
    free_expr(e->assign.expr);
    break;

  case IF:
    free_expr(e->if_expr.cond);
    free_expr(e->if_expr.e_true);
    free_expr(e->if_expr.e_false);
    break;

  case WHILE:
    free_expr(e->while_expr.cond);
    free_expr(e->while_expr.body);
    break;

  case UN_OP:
    free_expr(e->unop.expr);
    break;

  case BIN_OP:
    free_expr(e->binop.lhs);
    free_expr(e->binop.rhs);
    break;
  }

  free(e);
}

LLVMValueRef codegen_expr(
  struct expr *e,
  struct env *env,
  LLVMModuleRef module,
  LLVMBuilderRef builder
)
{
  switch (e->type) {
  case LITERAL: {
    return LLVMConstInt(LLVMInt32Type(), e->value, 0);
  }

  case LIT_BOOL: {
    return LLVMConstInt(LLVMInt1Type(), e->value, 0);
  }

  case CALL: {
    LLVMValueRef expr = codegen_expr(e->call.expr, env, module, builder);
    LLVMValueRef args[] = { expr };
    LLVMValueRef fn = LLVMGetNamedFunction(module, e->call.ident);
    if (!fn) {
      fprintf(stderr, "Undefined function: %s\n", e->call.ident);
      return expr;
    }
    return LLVMBuildCall(builder, fn, args, 1, "");
  }

  case LET: {
    LLVMValueRef expr = codegen_expr(e->let.expr, env, module, builder);
    struct env *new_env = push(env, e->let.ident, expr);
    LLVMValueRef body = codegen_expr(e->let.body, new_env, module, builder);
    pop(new_env);
    return body;
  }

  case VAR: {
    LLVMBasicBlockRef current_bb = LLVMGetInsertBlock(builder);
    LLVMValueRef f = LLVMGetBasicBlockParent(current_bb);
    LLVMBasicBlockRef entry_bb = LLVMGetEntryBasicBlock(f);
    LLVMValueRef expr = codegen_expr(e->var.expr, env, module, builder);

    // create the cell in the entry basic block of the function
    LLVMPositionBuilder(builder, entry_bb, LLVMGetFirstInstruction(entry_bb));
    LLVMValueRef pointer = LLVMBuildAlloca(builder, LLVMTypeOf(expr), e->var.ident);

    // return to the old builder position and continue from there
    LLVMPositionBuilderAtEnd(builder, current_bb);
    LLVMBuildStore(builder, expr, pointer);

    struct env *new_env = push(env, e->var.ident, pointer);
    LLVMValueRef body = codegen_expr(e->var.body, new_env, module, builder);
    pop(new_env);
    return body;
  }

  case ASSIGN: {
    LLVMValueRef expr = codegen_expr(e->var.expr, env, module, builder);
    LLVMValueRef pointer = resolve(env, e->assign.ident);
    return LLVMBuildStore(builder, expr, pointer);
  }

  case IDENT: {
    LLVMValueRef val = resolve(env, e->ident);
    if (LLVMGetTypeKind(LLVMTypeOf(val)) == LLVMPointerTypeKind) {
      return LLVMBuildLoad(builder, val, "");
    } else {
      return val;
    }
  }

  case IF: {
    LLVMValueRef f = LLVMGetBasicBlockParent(LLVMGetInsertBlock(builder));
    LLVMBasicBlockRef then_bb = LLVMAppendBasicBlock(f, "then");
    LLVMBasicBlockRef else_bb = LLVMAppendBasicBlock(f, "else");
    LLVMBasicBlockRef cont_bb = LLVMAppendBasicBlock(f, "cont");

    LLVMValueRef cond = codegen_expr(e->if_expr.cond, env, module, builder);
    LLVMBuildCondBr(builder, cond, then_bb, else_bb);

    LLVMPositionBuilderAtEnd(builder, then_bb);
    LLVMValueRef then_val = codegen_expr(e->if_expr.e_true, env, module, builder);
    LLVMBuildBr(builder, cont_bb);
    then_bb = LLVMGetInsertBlock(builder);

    LLVMPositionBuilderAtEnd(builder, else_bb);
    LLVMValueRef else_val = codegen_expr(e->if_expr.e_false, env, module, builder);
    LLVMBuildBr(builder, cont_bb);
    else_bb = LLVMGetInsertBlock(builder);

    LLVMPositionBuilderAtEnd(builder, cont_bb);

    LLVMTypeRef type = LLVMTypeOf(then_val);

    if (LLVMGetTypeKind(type) == LLVMVoidTypeKind) {
      return then_val; // void value, just return any expr of the appropriate type
    } else {
      LLVMValueRef phi = LLVMBuildPhi(builder, type, "");
      LLVMValueRef values[] = {then_val, else_val};
      LLVMBasicBlockRef blocks[] = {then_bb, else_bb};
      LLVMAddIncoming(phi, values, blocks, 2);
      return phi;
    }
  }

  case WHILE: {
    LLVMValueRef f = LLVMGetBasicBlockParent(LLVMGetInsertBlock(builder));
    LLVMBasicBlockRef cond_bb = LLVMAppendBasicBlock(f, "cond");
    LLVMBasicBlockRef body_bb = LLVMAppendBasicBlock(f, "body");
    LLVMBasicBlockRef cont_bb = LLVMAppendBasicBlock(f, "cont");

    LLVMValueRef ret = LLVMBuildBr(builder, cond_bb);

    LLVMPositionBuilderAtEnd(builder, cond_bb);
    LLVMValueRef cond = codegen_expr(e->while_expr.cond, env, module, builder);
    LLVMBuildCondBr(builder, cond, body_bb, cont_bb);

    LLVMPositionBuilderAtEnd(builder, body_bb);
    codegen_expr(e->while_expr.body, env, module, builder);
    LLVMBuildBr(builder, cond_bb);

    LLVMPositionBuilderAtEnd(builder, cont_bb);
    return ret; // return a void expression
  }

  case UN_OP: {
    LLVMValueRef expr = codegen_expr(e->unop.expr, env, module, builder);
    return LLVMBuildNot(builder, expr, "");
  }

  case BIN_OP: {
    LLVMValueRef lhs = codegen_expr(e->binop.lhs, env, module, builder);
    LLVMValueRef rhs = codegen_expr(e->binop.rhs, env, module, builder);
    switch (e->binop.op) {
    case '+': return LLVMBuildAdd(builder, lhs, rhs, "");
    case '-': return LLVMBuildSub(builder, lhs, rhs, "");
    case '*': return LLVMBuildMul(builder, lhs, rhs, "");
    case '/': return LLVMBuildSDiv(builder, lhs, rhs, "");
    case '<': return LLVMBuildICmp(builder, LLVMIntSLT, lhs, rhs, "");
    case '>': return LLVMBuildICmp(builder, LLVMIntSGT, lhs, rhs, "");
    case LE: return LLVMBuildICmp(builder, LLVMIntSLE, lhs, rhs, "");
    case GE: return LLVMBuildICmp(builder, LLVMIntSGE, lhs, rhs, "");
    case '=': return LLVMBuildICmp(builder, LLVMIntEQ, lhs, rhs, "");
    case NE: return LLVMBuildICmp(builder, LLVMIntNE, lhs, rhs, "");
    case '&': return LLVMBuildAnd(builder, lhs, rhs, "");
    case '|': return LLVMBuildOr(builder, lhs, rhs, "");
    default: return NULL;
    }
  }

  default:
    return NULL;
  }
}

void jit_eval(struct expr *expr) {
  LLVMModuleRef module = LLVMModuleCreateWithName("exe");
  LLVMBuilderRef builder = LLVMCreateBuilder();
  LLVMExecutionEngineRef engine;

  LLVMTypeRef one_i32_arg[] = {LLVMInt32Type()};
  LLVMAddFunction(module, "print_i32",
                  LLVMFunctionType(LLVMVoidType(), one_i32_arg, 1, 0));

  LLVMAddFunction(module, "read_i32",
                  LLVMFunctionType(LLVMInt32Type(), one_i32_arg, 1, 0));

  LLVMInitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();
  LLVMInitializeNativeAsmParser();
  LLVMLinkInMCJIT();

  char *error;
  if (LLVMCreateExecutionEngineForModule(&engine, module, &error)) {
    fprintf(stderr, "%s\n", error);
    return;
  }

  // LLVM can only emit instructions in basic blocks
  //   basic blocks are always part of a function
  //   function are contained in modules

  // visit expression to get its LLVM type
  LLVMTypeRef bad_f_type = LLVMFunctionType(LLVMVoidType(), NULL, 0, 0);
  LLVMValueRef typing_f = LLVMAddFunction(module, "typing_f", bad_f_type);
  LLVMBasicBlockRef typing_entry_bb = LLVMAppendBasicBlock(typing_f, "entry");
  LLVMPositionBuilderAtEnd(builder, typing_entry_bb);
  LLVMValueRef typing_ret = codegen_expr(expr, NULL, module, builder);
  LLVMBuildRetVoid(builder);
  LLVMTypeRef type = LLVMTypeOf(typing_ret);
  LLVMDeleteFunction(typing_f);

  // emit expression as function body
  LLVMTypeRef actual_f_type = LLVMFunctionType(type, NULL, 0, 0);
  LLVMValueRef f = LLVMAddFunction(module, "f", actual_f_type);
  LLVMBasicBlockRef entry_bb = LLVMAppendBasicBlock(f, "entry");
  LLVMPositionBuilderAtEnd(builder, entry_bb);
  LLVMValueRef ret = codegen_expr(expr, NULL, module, builder);

  // return the result and terminate the function
  if (LLVMGetTypeKind(type) == LLVMVoidTypeKind) {
    LLVMBuildRetVoid(builder);
  } else {
    LLVMBuildRet(builder, ret);
  }

  LLVMDumpValue(f);

  LLVMVerifyModule(module, LLVMAbortProcessAction, &error);

  fprintf(stderr, "running...\n");
  LLVMGenericValueRef result = LLVMRunFunction(engine, f, 0, NULL);

  if (LLVMGetTypeKind(type) == LLVMVoidTypeKind) {
    printf("-> done\n");
  } else {
    printf("-> %d\n", (int)LLVMGenericValueToInt(result, 0));
  }
  LLVMDisposeGenericValue(result);

  LLVMDisposeBuilder(builder);
  LLVMDisposeExecutionEngine(engine);
}