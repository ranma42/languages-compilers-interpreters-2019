            #define TOK_IDENT 1
            #define TOK_INT_LIT 2
            #define TOK_OP 3
            #define TOK_UNEXPECTED 4
            #define TOK_KEYWORD 5
            #define TOK_FLOAT_LIT 6

IDENT       [A-Za-z][A-Za-z0-9_]*
OP          [-+*{}()=/]
SINT        [-+]?[0-9]+
EXP         [eE]{SINT}

%%

if|else|for|int            return TOK_KEYWORD;
{IDENT}                    return TOK_IDENT;
[[:space:]]+               /* ignore */
{SINT}                     return TOK_INT_LIT;
{SINT}{EXP}                return TOK_FLOAT_LIT;
{SINT}\.[0-9]*{EXP}?       return TOK_FLOAT_LIT;
[-+]?[0-9]*\.[0-9]+{EXP}?  return TOK_FLOAT_LIT;
{OP}                       return TOK_OP;
.                          return TOK_UNEXPECTED;

%%

void handle_token(int token) {
    switch (token) {
      case TOK_IDENT:
        printf("Identifier: %s\n", yytext);
        return;

      case TOK_INT_LIT:
        printf("Integer: %s\n", yytext);
        return;

      case TOK_FLOAT_LIT:
        printf("Float: %s\n", yytext);
        return;

      case TOK_OP:
        printf("Operator: %s\n", yytext);
        return;

      case TOK_KEYWORD:
        printf("Keyword: %s\n", yytext);
        return;

      case TOK_UNEXPECTED:
        printf("Unexpected symbol: %s\n", yytext);
        return;
    }
}

int yywrap(void)
{
  return 1;
}

int main(void) {
  int tokentype;

  while ((tokentype = yylex())) {
    handle_token(tokentype);
  }

  return 0;
}
