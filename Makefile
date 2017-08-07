SRCS=$(wildcard src/*.cpp)
HDRS=$(wildcard src/*.h)
CSRC=base64/base64.c
ALL_OBJS=$(subst .cpp,.o,$(SRCS)) $(subst .c,.o,$(CSRC))
MAINS=src/main.o test/test_score_eval.o
OBJS=$(filter-out $(MAINS),$(ALL_OBJS)) # remove objs with a "main"
CC=clang-3.9
CXX=clang++-3.9
FLAGS=# -ferror-limit=1
LIBS=-lboost_iostreams -lboost_system -lz

all: bin/main bin/graphio bin/test_score_eval bin/testgame bin/moveio bin/testmap bin/fightgame bin/testdandelion

DBG_FLAGS=-pg

%.o:: %.c
	gcc $(DBG_FLAGS) -c -o $@ $<

%.o:: $(HDRS)

%.o:: %.cpp
	$(CXX) -I./base64 -std=c++1z $(DBG_FLAGS) -c -o $@ $<

bin/main: src/main.o src/*.cpp $(OBJS) $(HDRS) 
	$(CXX) $(FLAGS) -I./base64 -std=c++1z -o $@ $(DBG_FLAGS) -lstdc++ $< $(OBJS) $(LIBS)

bin/graphio: test/graphio.cpp $(HDRS)
	$(CXX) -I./base64 -Isrc -std=c++1z $(DBG_FLAGS) -lstdc++ -o $@ $< $(OBJS) $(LIBS)

bin/moveio: test/moveio.cpp $(HDRS)
	$(CXX) -I./base64 -Isrc -std=c++1z $(DBG_FLAGS) -lstdc++ -o $@ $< $(OBJS) $(LIBS)

bin/test_score_eval: test/test_score_eval.cpp src/score_eval.o $(HDRS)
	$(CXX) $(FLAGS) -I./base64 -Isrc -std=c++1z $(DBG_FLAGS) -lstdc++ -o $@ $< $(OBJS) $(LIBS)

bin/testgame: test/testgame.cpp src/plan.o src/game.o $(HDRS)
	$(CXX) -I./base64 -Isrc -std=c++1z $(DBG_FLAGS) -lstdc++ -o $@ test/testgame.cpp $(OBJS) $(LIBS)

bin/testmap: test/testmap.cpp src/plan.o src/game.o $(HDRS)
	$(CXX) -I./base64 -Isrc -std=c++1z $(DBG_FLAGS) -lstdc++ -o $@ test/testmap.cpp $(OBJS) $(LIBS)

bin/fightgame: test/fightgame.cpp $(OBJS) $(HDRS)
	$(CXX) -I./base64 -Isrc -std=c++1z $(DBG_FLAGS) -lstdc++ -o $@ test/fightgame.cpp $(OBJS) $(LIBS)

bin/testdandelion: test/testdandelion.cpp $(OBJS) $(HDRS)
	$(CXX) -I./base64 -Isrc -std=c++1z $(DBG_FLAGS) -lstdc++ -o $@ test/testdandelion.cpp $(OBJS) $(LIBS)

bin:
	mkdir -p bin

clean:
	rm -rf */*.o bin/*
