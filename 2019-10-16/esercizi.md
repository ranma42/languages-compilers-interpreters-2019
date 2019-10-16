# Esercizio

Modifica il parser di `arith.y` affinché generi una rappresentazione assembly delle espressioni.

Le istruzioni disponibili sono:

```
load rN, NUM            # imposta il valore del registro N a NUM
add  rA, rB, rC         # imposta il valore del registro A alla somma dei valori dei registri B e C
sub  rA, rB, rC         # imposta il valore del registro A alla differenza dei valori dei registri B e C
mul  rA, rB, rC         # imposta il valore del registro A al prodotto dei valori dei registri B e C
div  rA, rB, rC         # imposta il valore del registro A al quoziente dei valori dei registri B e C
```

I registri disponibili sono identificati con la notazione `rN`, dove `N` è un numero naturale.
`r0`, `r9`, `r12345` sono tutti esempi di registri validi.
