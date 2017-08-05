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
    std::string start_game
        ((std::istreambuf_iterator<char>(gamefile)),
         std::istreambuf_iterator<char>());
    std::string continue_game = start_game;

    int numEdges = -1;

    std::map<int, std::string> continue_state;
    std::map<int, Move> moves_since_last;

    for (int i = 0; i != numEdges; i++) {
        std::istringstream begin_game(continue_game);
        {
            Opening o;
            begin_game >> o;
            if (o.ot == SetupOp) {
                auto edges = o.setup.map.getEdges();
                numEdges = edges.size();
                auto streamAt = begin_game.tellg();
                std::string nextWord;
                begin_game >> nextWord;
                begin_game.seekg(streamAt);
                std::cerr << "begin_game: at " << streamAt << " next " << nextWord << "\n";
                begin_game >> o.setup.moves;
            } else {
                OurState os(o);
                begin_game >> os;
            }

            auto whichPlayer = i % numPlayers;
            o.setup.punter = whichPlayer;
            
            auto take_move = o.run();
            o.setup.moves.insert(take_move);
            moves_since_last[whichPlayer] = take_move;

            std::ostringstream restart_oss;
            restart_oss << OurState(o);
            continue_state[whichPlayer] = restart_oss.str();

            std::ostringstream game_oss;

            auto nextPlayer = (i + 1) % numPlayers;
            
            auto continue_it = continue_state.find(nextPlayer);
            if (continue_it == continue_state.end()) {
                game_oss << "setup " << o.setup << "\n";
                for (auto &it : moves_since_last) {
                    if (it.first != nextPlayer) {
                        game_oss << it.second << " ";
                    }
                }
                game_oss << "end\n";
            } else {
                game_oss << "move ";
                for (auto &it : moves_since_last) {
                    if (it.first != nextPlayer) {
                        game_oss << it.second << " ";
                    }
                }
                game_oss << "end\n";
                game_oss << continue_it->second;
            }
            
            continue_game = game_oss.str();
            std::cout << "continue " << continue_game << "\n";
        }
    }

    for (auto i = 0; i < numPlayers; i++) {
        Opening last;
        OurState os(last);
        auto continue_game = continue_state[i];
        std::istringstream last_read("move end\n" + continue_game);
        last_read >> last;
        last_read >> os;
        auto score = score_player_map(i, last.setup.map);
        std::cout << "player " << i << " score " << score << "\n";
    }
    
    return 0;
}
