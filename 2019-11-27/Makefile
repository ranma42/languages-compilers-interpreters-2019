CFLAGS=`llvm-config --cflags`
LLVM_LINK_FLAGS=`llvm-config --libs --cflags --ldflags core analysis executionengine mcjit interpreter native --system-libs`

LEX?=flex
YACC?=bison
YFLAGS?=-d

all: eval

scanner.o: parser.c

ast.o: parser.c

eval: scanner.o parser.o ast.o
	$(CXX) $(LLVM_LINK_FLAGS) -o $@ $^

clean:
	rm -f eval ast.o scanner.o parser.o parser.c y.tab.h
