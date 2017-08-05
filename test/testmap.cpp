#include <iostream>
#include <fstream>
#include "ourgraph.h"
#include "game.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "usage: testmap map.crk\n";
        return 1;
    }
    std::ifstream gamefile(argv[1]);
    std::string continue_game
        ((std::istreambuf_iterator<char>(gamefile)),
         std::istreambuf_iterator<char>());
    std::string game_state;
    int numEdges = -1;

    std::string command;
    std::cin >> command;
    std::istringstream begin_game(continue_game);
    Opening o;
    begin_game >> o;
    
    while (command != "end") {
        if (command == "edges-away") {
            std::string vtx;
            std::cin >> vtx;
            auto edgesAway = o.setup.map.getEdgesAway(vtx);
            for (auto &it : edgesAway) {
                std::cout << it.first << "," << it.second << "\n";
            }
        }
        std::cout << o.setup.map;
    }
    return 0;
}
