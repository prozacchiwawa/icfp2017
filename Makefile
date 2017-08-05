SRCS=$(wildcard src/*.cpp)
HDRS=$(wildcard src/*.h)
CSRC=base64/base64.c
ALL_OBJS=$(subst .cpp,.o,$(SRCS)) $(subst .c,.o,$(CSRC))
MAINS=src/main.o test/test_score_eval.o
OBJS=$(filter-out $(MAINS),$(ALL_OBJS)) # remove objs with a "main"
CC=clang-3.9
CXX=clang++-3.9
B64=base64/base64.o
FLAGS=# -ferror-limit=1

all: bin/main bin/graphio bin/test_score_eval bin/testgame bin/moveio bin/testmap bin/fightgame

%.o:: %.c
	gcc -g -c -o $@ $<

%.o:: $(HDRS)

%.o:: %.cpp
	$(CXX) -I./base64 -std=c++1z -g -c -o $@ $<

bin/main: src/main.o src/*.cpp $(OBJS) $(HDRS) 
	$(CXX) $(FLAGS) -I./base64 -std=c++1z -o $@ -g -lstdc++ $< $(OBJS) 

bin/graphio: test/graphio.cpp $(HDRS)
	$(CXX) -I./base64 -Isrc -std=c++1z -g -lstdc++ -o $@ $<

bin/moveio: test/moveio.cpp $(HDRS)
	$(CXX) -I./base64 -Isrc -std=c++1z -g -lstdc++ -o $@ $< $(OBJS)

bin/test_score_eval: test/test_score_eval.cpp src/score_eval.o $(HDRS)
	$(CXX) $(FLAGS) -I./base64 -Isrc -std=c++1z -g -lstdc++ -o $@ $< $(OBJS)

bin/testgame: test/testgame.cpp src/plan.o src/game.o $(HDRS)
	$(CXX) -I./base64 -Isrc -std=c++1z -g -lstdc++ -o $@ test/testgame.cpp src/plan.o src/game.o $(B64)

bin/testmap: test/testmap.cpp src/plan.o src/game.o $(HDRS)
	$(CXX) -I./base64 -Isrc -std=c++1z -g -lstdc++ -o $@ test/testmap.cpp src/plan.o src/game.o $(B64)

bin/fightgame: test/fightgame.cpp src/plan.o src/game.o src/score_eval.o $(HDRS)
	$(CXX) -I./base64 -Isrc -std=c++1z -g -lstdc++ -o $@ test/fightgame.cpp src/plan.o src/game.o src/score_eval.o $(B64)

bin:
	mkdir -p bin

clean:
	rm -rf */*.o bin
