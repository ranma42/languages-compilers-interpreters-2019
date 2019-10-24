enum expr_type {
  LITERAL,
  IDENT,
  BIN_OP,
};

struct expr {
  enum expr_type type;
  union {
    int value;

    char *ident;

    struct {
      struct expr *lhs;
      struct expr *rhs;
      char op;
    } binop;
  };
};

struct expr *make_val(int value);
struct expr *make_identifier(char *ident);
struct expr *make_bin_op(struct expr *lhs, char op, struct expr *rhs);
int print_expr(struct expr *e);
void free_expr(struct expr *e);
