#include <iostream>
#include <fstream>
#include "ourgraph.h"
#include "game.h"
#include "score_eval.h"

int main(int argc, char **argv) {
    if (argc < 4) {
        std::cerr << "usage: fightgame <id> <out-of> map.crk\n";
        return 1;
    }

    PID id;
    int numPlayers;
    std::istringstream iss_id(argv[1]);
    std::istringstream iss_outof(argv[2]);
    iss_id >> id;
    iss_outof >> numPlayers;
    std::ifstream gamefile(argv[3]);
    std::string continue_game
        ((std::istreambuf_iterator<char>(gamefile)),
         std::istreambuf_iterator<char>());
    std::string game_state;

    int numEdges = -1;
    int whichPlayer = 0;

    std::vector<uint64_t> ends;

    for (int i = 0; i != numEdges; i++) {
        std::istringstream begin_game(continue_game);
        auto lastPlayer = whichPlayer;
        
        {
            Opening o;
            begin_game >> o;
            if (o.ot == SetupOp) {
                auto edges = o.setup.map.getEdges();
                numEdges = edges.size();
                ends.resize(o.setup.punters);
            } else {
                OurState os(o);
                begin_game >> os;
            }
            
            auto take_move = o.run();
            std::ostringstream game_oss;
            std::ostringstream restart_oss;

            whichPlayer = (whichPlayer + 1) % numPlayers;
            
            o.setup.moves.insert(take_move);
            o.setup.punter = whichPlayer;
            restart_oss
                << "move " << o.setup.moves
                << OurState(o) << "\n";

            continue_game = restart_oss.str();
            std::cout << "continue " << continue_game << "\n";
        }
        
        if (i + numPlayers >= numEdges) {
            Opening last;
            OurState os(last);
            std::istringstream last_read(continue_game);
            last_read >> last;
            last_read >> os;
            ends[lastPlayer] = score_player_map(lastPlayer, last.setup.map);
        }
    }

    int i = 0;
    for (auto &it : ends) {
        std::cout << "player " << i++ << " score " << it << "\n";
    }
    
    return 0;
}
