# Introduction to `flex`

 - `example1-count.l` is a simple scanner which counts the characters and lines of the input stream

 - `example2-csv-naive.l` splits the input stream in records (at newlines) and fields (at commas)

 - `example3-csv-defs.l` is the same as `example2` except for using definitions

 - `example4-csv-rfc4180.l` extends `example3` with support for quoted fields (based on [RFC4180](https://tools.ietf.org/html/rfc4180)) through starting conditions

Assuming that `flex` is available in the environment, each of these files can be transformed into the C source code for a complete program and then compiled by running:

```sh
flex exampleN-description.l # generate lex.yy.c
cc lex.yy.c # compile it to a.out
```

The `a.out` program accepts its input from the standard input stream, therefore it is possible to run it on an input file using input redirection. For example, to compile the first example and run it on this `README.md` file you could run

```sh
flex example1-count.l
cc lex.yy.c
./a.out < README.md
```

# Need help?

A comprehensive guide to `flex` is available [here](http://web.mit.edu/gnu/doc/html/flex_toc.html).

The `flex` tool is available as a package on most Linux distributions and it can be installed on MacOSX through the homebrew [`flex` formula](https://formulae.brew.sh/formula/flex).

If you need further help, get in touch by e-mail :)
