# Exercise

Modify the parser in `arith.y` to emit the expressions in an assembly-like representation.

The instruction set is the following one:

```
load rN, NUM            # sets the value of register N to NUM
add  rA, rB, rC         # sets the value of register A to the sum of the values from register B and C
sub  rA, rB, rC         # sets the value of register A to the difference of the values from register B and C
mul  rA, rB, rC         # sets the value of register A to the product of the values from register B and C
div  rA, rB, rC         # sets the value of register A to the quotient of the values from register B and C
```

The available registers are identified by `rN` where `N` is a natural number.
For example `r0`, `r9` and `r12345` are all valid registers.
