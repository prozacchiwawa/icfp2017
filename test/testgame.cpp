#include <iostream>
#include <fstream>
#include "ourgraph.h"
#include "game.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "usage: testgame map.crk\n";
        return 1;
    }
    std::ifstream gamefile(argv[1]);
    std::string continue_game
        ((std::istreambuf_iterator<char>(gamefile)),
         std::istreambuf_iterator<char>());
    std::string game_state;
    int numEdges = -1;
    for (int i = 0; i != numEdges; i++) {
        std::istringstream begin_game(continue_game);
        {
            Opening o;
            begin_game >> o;
            if (o.ot == SetupOp) {
                auto edges = o.setup.map.getEdges();
                numEdges = edges.size();
                begin_game >> o.setup.moves;
                o.setupFinalize();
            } else {
                readEncodedSetup(begin_game, o);
            }
            auto take_move = o.run();
            std::ostringstream game_oss;
            std::ostringstream restart_oss;
            o.addMove(take_move.punter, take_move.claimMove.source, take_move.claimMove.target);
            restart_oss
                << "move " << o.setup.moves << " ";
            writeEncodedSetup(restart_oss, o);
            restart_oss << "\n";
            continue_game = restart_oss.str();
            std::cout << "continue " << continue_game << "\n";
        }
    }
    return 0;
}
