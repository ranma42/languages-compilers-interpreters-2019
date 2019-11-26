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
struct expr *make_bin_op(struct expr *lhs, int op, struct expr *rhs) {
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

  case LIT_BOOL: {
    int dest = gen_reg();
    printf("load r%d, %s\n", dest, e->value ? "true" : "false");
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
    case '<':
      printf("lt r%d, r%d, r%d\n", dest, lhs, rhs);
      break;
    case '>':
      printf("gt r%d, r%d, r%d\n", dest, lhs, rhs);
      break;
    case LE:
      printf("le r%d, r%d, r%d\n", dest, lhs, rhs);
      break;
    case GE:
      printf("ge r%d, r%d, r%d\n", dest, lhs, rhs);
      break;
    case '=':
      printf("eq r%d, r%d, r%d\n", dest, lhs, rhs);
      break;
    case NE:
      printf("ne r%d, r%d, r%d\n", dest, lhs, rhs);
      break;
    }
    return dest;
  }
  }
}

// Dump the stack machine version of the expression
void print_expr_stack(struct expr *e) {
  switch (e->type) {
  case LITERAL: {
    printf("load %d\n", e->value);
    return;
  }

  case LIT_BOOL: {
    printf("load_%s\n", e->value ? "true" : "false");
    return;
  }

  case IDENT: {
    printf("load %s\n", e->ident);
    return;
  }

  case BIN_OP: {
    print_expr_stack(e->binop.lhs);
    print_expr_stack(e->binop.rhs);
    switch (e->binop.op) {
    case '+':
      printf("add\n");
      break;
    case '-':
      printf("sub\n");
      break;
    case '*':
      printf("mul\n");
      break;
    case '/':
      printf("div\n");
      break;
    case '<':
      printf("lt\n");
      break;
    case '>':
      printf("gt\n");
      break;
    case LE:
      printf("le\n");
      break;
    case GE:
      printf("ge\n");
      break;
    case '=':
      printf("eq\n");
      break;
    case NE:
      printf("ne\n");
      break;
    }
    return;
  }
  }
}

void free_expr(struct expr *e) {
  switch (e->type) {
  case LITERAL:
  case LIT_BOOL:
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

enum value_type check_types(struct expr *e) {
  switch (e->type) {
  case IDENT: {
    return ERROR; // TODO: get type from environment
  }

  case LITERAL: {
    return INTEGER;
  }

  case LIT_BOOL: {
    return BOOLEAN;
  }

  case BIN_OP: {
    enum value_type lhs = check_types(e->binop.lhs);
    enum value_type rhs = check_types(e->binop.rhs);
    switch (e->binop.op) {
    case '+':
    case '-':
    case '*':
    case '/': {
      if (lhs == INTEGER && rhs == INTEGER) {
        return INTEGER;
      } else {
        return ERROR;
      }
    }
    case '<':
    case '>':
    case LE:
    case GE: {
      if (lhs == INTEGER && rhs == INTEGER) {
        return BOOLEAN;
      } else {
        return ERROR;
      }
    }
    case '=':
    case NE: {
      if (lhs == ERROR || rhs == ERROR || lhs != rhs) {
        return ERROR;
      } else {
        return BOOLEAN;
      }
    }

    default:
      return ERROR;
    }
  }
  }
}
