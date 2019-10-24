LEX?=flex
YACC?=bison
YFLAGS?=-d

all: arith

scanner.o: parser.c

arith: scanner.o parser.o
	$(CC) -o $@ $^

clean:
	rm -f arith scanner.o parser.o parser.c y.tab.h
