#include <iostream>
#include "ourgraph.h"
#include "game.h"

int main() {
    std::string continue_game("setup 0 2 0 1 2 3 end 0 1 1 3 0 2 2 3 end 0 end");
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
            } else {
                OurState os(o);
                begin_game >> os;
            }
            auto take_move = o.run();
            std::ostringstream game_oss;
            std::ostringstream restart_oss;
            o.setup.moves.insert(take_move);
            restart_oss
                << "move " << o.setup.moves
                << OurState(o) << "\n";
            continue_game = restart_oss.str();
            std::cout << "continue " << continue_game << "\n";
        }
    }
    return 0;
}
