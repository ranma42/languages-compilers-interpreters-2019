# Exercises

Use `flex` to generate 3 scanners that can:

1. tokenize a (whitespace-separated) sequence of decimal floating-point numbers.

   Example input:
```
10 0.7 1.3e07 -37
-0 2. .27 1.5E+9 150E-9
```

2. tokenize a sequence of C-like
   strings (replacing escape sequences with the corresponding character)

   Example input:
```
"Hello\n\tGoodbye"
"Some\"special\'characters.*\\"
"A simple string"
```

3. escape a sequence of bytes into
   a C-like string; this scanner should be able to accept any input (not containing a NUL, aka '\0')
   and ideally it should output user-friendly strings
