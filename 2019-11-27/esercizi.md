# Esercizi

Extra: Implementare la generazione di codice (LLVM IR) per espressioni `LET` and `IDENT`.

L'approccio consigliato è di implementare una struttura dati "ambiente" che supporti le seguenti operazioni:

```C
// cerca il valore associato a `name`
LLVMValueRef resolve(struct env *env, char *name);

// aggiungi l'associazione `name` -> `value` ad `env`
struct env *push(struct env *env, char *name, LLVMValueRef value);

// rimuovi l'associazione "in cima" (la più recente) da `env`
struct env *pop(struct env *env);
```
