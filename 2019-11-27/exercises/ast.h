enum expr_type {
  LITERAL,
  LIT_BOOL,
  IDENT,
  UN_OP,
  BIN_OP,
};

enum value_type {
  ERROR,
  INTEGER,
  BOOLEAN,
};

struct expr {
  enum expr_type type;
  union {
    int value;

    char *ident;

    struct {
      struct expr *expr;
      int op;
    } unop;

    struct {
      struct expr *lhs;
      struct expr *rhs;
      int op;
    } binop;
  };
};

struct expr *make_val(int value);
struct expr *make_bool(int value);
struct expr *make_identifier(char *ident);
struct expr *make_un_op(int op, struct expr *expr);
struct expr *make_bin_op(struct expr *lhs, int op, struct expr *rhs);
int print_expr(struct expr *e);
void print_expr_stack(struct expr *e);
void free_expr(struct expr *e);
enum value_type check_types(struct expr *e);
