# Esercizi

Utilizza `flex` per generare 3 scanner in grado di:

1. suddividere in token un sequenza di numeri in virgola mobile (separati da caratteri di spaziatura).

   Input di esempio:
```
10 0.7 1.3e07 -37
-0 2. .27 1.5E+9 150E-9
```

2. suddividere in token un sequenza di stringhe "C"
   (sostituendo le sequenze di escape con il carattere correspondente)

   Input di esempio:
```
"Hello\n\tGoodbye"
"Some\"special\'characters.*\\"
"A simple string"
```

3. trasformare una sequenza di bytes in una stringa "C";
   lo scanner dovrebbe essere in grado di gestire input qualunque (che non contengano NUL, aka '\0'),
   idealmente generando delle stringhe in un formato "user-friendly"
