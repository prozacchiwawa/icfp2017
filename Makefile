SRCS=$(wildcard src/*.cpp)
HDRS=$(wildcard src/*.h)
CSRC=base64/base64.c
OBJS=$(subst .cpp,.o,$(SRCS)) $(subst .c,.o,$(CSRC))

all: bin/main bin/graphio

%.o:: %.c
	gcc -g -c -o $@ $<

%.o:: $(HDRS)

%.o:: %.cpp
	g++ -I./base64 -std=c++1z -g -c -o $@ $<

bin/main: bin src/*.cpp $(OBJS) $(HDRS)
	g++ -I./base64 -std=c++1z -o $@ -g -lstdc++ $(OBJS)

bin/graphio: test/graphio.cpp $(HDRS)
	g++ -I./base64 -Isrc -std=c++1z -g -lstdc++ -o $@ $<

bin:
	mkdir -p bin

clean:
	rm -rf */*.o bin
