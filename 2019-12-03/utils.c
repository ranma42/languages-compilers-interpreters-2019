#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_i32(int x)
{
  printf("%d\n", x);
}

int read_i32(int defaultValue) {
  int x;
  if (scanf("%d", &x)) {
    return x;
  } else {
    return defaultValue;
  }
}

LLVMValueRef resolve(struct env *env, char *name) {
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
