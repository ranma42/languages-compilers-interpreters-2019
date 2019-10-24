#include <stdio.h>
#include <stdlib.h>

#include "ast.h"

static int reg_index = 0;

static int gen_reg() { return reg_index++; }

struct expr *make_val(int value) {
  struct expr *e = malloc(sizeof(struct expr));

  e->type = LITERAL;
  e->value = value;

  return e;
}

struct expr *make_identifier(char *ident) {
  struct expr *e = malloc(sizeof(struct expr));

  e->type = IDENT;
  e->ident = ident;

  return e;
}
struct expr *make_bin_op(struct expr *lhs, char op, struct expr *rhs) {
  struct expr *e = malloc(sizeof(struct expr));

  e->type = BIN_OP;
  e->binop.lhs = lhs;
  e->binop.op = op;
  e->binop.rhs = rhs;

  return e;
}

// Dump the register machine version of the expression
int print_expr(struct expr *e) {
  switch (e->type) {
  case LITERAL: {
    int dest = gen_reg();
    printf("load r%d, %d\n", dest, e->value);
    return dest;
  }

  case IDENT: {
    int dest = gen_reg();
    printf("load r%d, %s\n", dest, e->ident);
    return dest;
  }

  case BIN_OP: {
    int lhs = print_expr(e->binop.lhs);
    int rhs = print_expr(e->binop.rhs);
    int dest = gen_reg();
    switch (e->binop.op) {
    case '+':
      printf("add r%d, r%d, r%d\n", dest, lhs, rhs);
      break;
    case '-':
      printf("sub r%d, r%d, r%d\n", dest, lhs, rhs);
      break;
    case '*':
      printf("mul r%d, r%d, r%d\n", dest, lhs, rhs);
      break;
    case '/':
      printf("div r%d, r%d, r%d\n", dest, lhs, rhs);
      break;
    }
    return dest;
  }
  }
}

void free_expr(struct expr *e) {
  switch (e->type) {
  case LITERAL:
    break;

  case IDENT:
    free(e->ident);
    break;

  case BIN_OP:
    free_expr(e->binop.lhs);
    free_expr(e->binop.rhs);
    break;
  }

  free(e);
}
