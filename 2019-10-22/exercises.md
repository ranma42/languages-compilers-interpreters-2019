# Exercises

Implement the stack-machine version of `print_expr`, i.e. a function that dumps the expression tree as a sequence of simple operations such as:

```
load NUM    # push NUM on the top of the stack
load ID     # push the value associated with ID on the top of the stack
add         # pop two values off the stack and push their sum
sub         # pop two values off the stack and push their difference
mul         # pop two values off the stack and push their product
div         # pop two values off the stack and push their quotient
```
