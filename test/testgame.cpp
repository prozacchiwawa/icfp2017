#include <iostream>
#include "ourgraph.h"
#include "game.h"

int main() {
    Opening o;
    OurState s;
    std::string continue_game("setup 0 2 0 1 2 3 end 0 1 1 3 0 2 2 3 end 0 end");
    std::string game_state;
    int numEdges = -1;
    for (int i = 0; i != numEdges; i++) {
        std::istringstream begin_game(continue_game);
        begin_game >> o;
        if (o.ot == SetupOp) {
            s.setup = o.setup;
            auto edges = s.setup.map.getEdges();
            numEdges = edges.size();
        }
        auto take_move = s.run();
        std::ostringstream game_oss;
        std::ostringstream restart_oss;
        restart_oss
            << "move " << s.setup.moves << " " << take_move << " end\n"
            << s << "\n";
        continue_game = restart_oss.str();
        std::cout << "continue " << continue_game << "\n";
    }
    return 0;
}
