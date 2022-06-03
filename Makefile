# Figure out what the name of the llvm-config executable is (assuming a limited
# number of options for the common environments in which we'll be building this
# code for this course).
LLVM_CONFIG := $(shell which llvm-config-7.0-64)
CFLAGS := -std=c++11
ifndef LLVM_CONFIG
	LLVM_CONFIG := $(shell which llvm-config-13)
	CFLAGS := -std=c++14
endif
ifndef LLVM_CONFIG
	LLVM_CONFIG := llvm-config
	CFLAGS := -std=c++14
endif

all: compile

parser.cpp parser.hpp: parser.y
	bison -d -o parser.cpp parser.y

scanner.cpp: scanner.l
	flex -o scanner.cpp scanner.l

compile: main.cpp ast.cpp ast.hpp parser.cpp scanner.cpp
	g++ ${CFLAGS} main.cpp ast.cpp parser.cpp scanner.cpp \
		-Wno-deprecated-register \
		$(shell $(LLVM_CONFIG) --cppflags --ldflags --libs --system-libs all) \
		-o compile

clean:
	rm -f compile target.o scanner.cpp parser.cpp parser.hpp

test:
	rm -f compile target.o scanner.cpp parser.cpp parser.hpp target target.ll target.o
	git pull
	flex -o scanner.cpp scanner.l
	bison -d -o parser.cpp parser.y
	g++ ${CFLAGS} main.cpp ast.cpp parser.cpp scanner.cpp \
		-Wno-deprecated-register \
		$(shell $(LLVM_CONFIG) --cppflags --ldflags --libs --system-libs all) \
		-o compile
	./compile < ./tests/python/test.py
# ./tests/python/test.py > ./compile > target.ll
# llc -filetype=obj target.ll
# gcc target.c target.o -o target
# ./target