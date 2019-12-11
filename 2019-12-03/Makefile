CFLAGS=`llvm-config --cflags`
LLVM_LINK_FLAGS=`llvm-config --libs --cflags --ldflags core analysis executionengine mcjit interpreter native --system-libs`

LEX?=flex
YACC?=bison
YFLAGS?=-d

all: eval

scanner.o: parser.c

ast.o: parser.c

eval: scanner.o parser.o ast.o utils.o
	$(CXX) -o $@ $^ $(LLVM_LINK_FLAGS) -rdynamic

clean:
	rm -f eval ast.o scanner.o parser.o utils.o parser.c y.tab.h
