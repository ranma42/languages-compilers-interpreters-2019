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

struct expr *make_let(char *ident, struct expr *expr, struct expr *body) {
  struct expr *e = malloc(sizeof(struct expr));

  e->type = LET;
  e->let.ident = ident;
  e->let.expr = expr;
  e->let.body = body;

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

  case LET:
    free(e->let.ident);
    free_expr(e->let.expr);
    free_expr(e->let.body);
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
  LLVMModuleRef module,
  LLVMBuilderRef builder
)
{
  switch (e->type) {
  case LITERAL: {
    // This could be implemented as
    // return LLVMConstInt(LLVMInt32Type(), e->value, 0);
    // but LLVM automatically constant-folds literals, therefore this would
    // make most expressions disappear from the generated assembly
    // We force the emission of explicit operations by storing the literal value in
    // a memory allocation and reading it back.
    LLVMValueRef pointer = LLVMBuildAlloca(builder, LLVMInt32Type(), "");
    LLVMBuildStore(builder, LLVMConstInt(LLVMInt32Type(), e->value, 0), pointer);
    return LLVMBuildLoad(builder, pointer, "");
  }

  case LIT_BOOL: {
    // Same trick as for the LITERAL case
    LLVMValueRef pointer = LLVMBuildAlloca(builder, LLVMInt1Type(), "");
    LLVMBuildStore(builder, LLVMConstInt(LLVMInt1Type(), e->value, 0), pointer);
    return LLVMBuildLoad(builder, pointer, "");
  }

  case LET: {
    return NULL;
  }

  case IDENT: {
    return NULL;
  }

  case UN_OP: {
    LLVMValueRef expr = codegen_expr(e->unop.expr, module, builder);
    return LLVMBuildNot(builder, expr, "");
  }

  case BIN_OP: {
    LLVMValueRef lhs = codegen_expr(e->binop.lhs, module, builder);
    LLVMValueRef rhs = codegen_expr(e->binop.rhs, module, builder);
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
