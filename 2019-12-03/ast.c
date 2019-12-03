#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "y.tab.h"

static int reg_index = 0;

static int gen_reg() { return reg_index++; }

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

  case IDENT: {
    return resolve(env, e->ident);
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

    LLVMPositionBuilderAtEnd(builder, else_bb);
    LLVMValueRef else_val = codegen_expr(e->if_expr.e_false, env, module, builder);
    LLVMBuildBr(builder, cont_bb);

    LLVMPositionBuilderAtEnd(builder, cont_bb);
    LLVMValueRef phi = LLVMBuildPhi(builder, LLVMTypeOf(then_val), "");
    LLVMValueRef values[] = { then_val, else_val };
    LLVMBasicBlockRef blocks[] = { then_bb, else_bb };
    LLVMAddIncoming(phi, values, blocks, 2);
    return phi;
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
    return ret;
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
    }
  }
    return NULL;
  }
}
