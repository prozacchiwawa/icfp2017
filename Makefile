SRCS=$(wildcard src/*.cpp)
OBJS=$(subst .cpp,.o,$(SRCS))

all: bin/main

bin/main: bin src/*.cpp
	clang -std=c++11 -o $@ -g -lstdc++ $(SRCS)

bin:
	mkdir -p bin
