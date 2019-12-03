#include "utils.h"
#include <stdlib.h>
#include <string.h>

LLVMValueRef resolve(struct env *env, char *name)
{
  if (env == NULL) {
    return NULL;
  } else if (strcmp(env->name, name) == 0) {
    return env->value;
  } else {
    return resolve(env->prev, name);
  }
}

struct env *push(struct env *env, char *name, LLVMValueRef value)
{
  struct env *r = malloc(sizeof(struct env));

  r->name = name;
  r->prev = env;
  r->value = value;

  return r;
}

// assumes that env is NOT NULL
struct env *pop(struct env *env)
{
  struct env *r = env->prev;
  free(env);
  return r;
}
