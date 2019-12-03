#include <llvm-c/Core.h>

struct env {
  struct env *prev;

  char *name;
  LLVMValueRef value;
};

LLVMValueRef resolve(struct env *env, char *name);
struct env *push(struct env *env, char *name, LLVMValueRef value);
struct env *pop(struct env *env);
