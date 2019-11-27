# Exercises

Extra: Implement the codegen for `LET` and `IDENT`.

The suggested approach is to implement an environment data structure that supports the following operations:

```C
// lookup the value bound to `name`
LLVMValueRef resolve(struct env *env, char *name);

// create a new environment by adding the `name` -> `value` binding to `env`
struct env *push(struct env *env, char *name, LLVMValueRef value);

// remove the "topmost" (latest) binding from `env`
struct env *pop(struct env *env);
```
