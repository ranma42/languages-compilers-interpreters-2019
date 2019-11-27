LEX?=flex
YACC?=bison
YFLAGS?=-d

all: eval

scanner.o: parser.c

ast.o: parser.c

eval: scanner.o parser.o ast.o
	$(CC) -o $@ $^

clean:
	rm -f eval ast.o scanner.o parser.o parser.c y.tab.h
