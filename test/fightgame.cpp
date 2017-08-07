#include <iostream>
#include <fstream>
#include "ourgraph.h"
#include "game.h"
#include "score_eval.h"

int main(int argc, char **argv) {
    if (argc < 4) {
        std::cerr << "usage: fightgame <id> <out-of> map.crk <max_moves>\n";
        return 1;
    }

    PID id;
    int max_moves = -1;
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

    if (argc > 4) {
      std::istringstream max_moves_str(argv[4]);
      max_moves_str >> max_moves;
    }

    int numEdges = -1;

    std::map<int, std::string> continue_state;
    std::map<int, Move> moves_since_last;

    std::cout << "continue " << continue_game << "\n";

    for (int i = 0; i != numEdges && i != max_moves; i++) {
        auto whichPlayer = i % numPlayers;
        std::istringstream begin_game(continue_game);
        {
            Opening o;
            begin_game >> o;
            if (o.ot == SetupOp) {
                o.setup.punter = whichPlayer;
                o.setupFinalize();
                auto edges = o.setup.map.getEdges();
                numEdges = edges.size();
                auto streamAt = begin_game.tellg();
                std::string nextWord;
                begin_game >> nextWord;
                begin_game.seekg(streamAt);
                std::cerr << "begin_game: at " << streamAt << " next " << nextWord << "\n";
                begin_game >> o.setup.moves;
            } else {
                o.setup.punter = whichPlayer;
                readEncodedSetup(begin_game, o);
            }
            
            auto take_move = o.run();
            if (take_move.moveType == Claim) {
                o.addMove(take_move.punter, take_move.claimMove.source, take_move.claimMove.target);
            }
            moves_since_last[whichPlayer] = take_move;

            std::ostringstream restart_oss;
            writeEncodedSetup(restart_oss, o);
            continue_state[whichPlayer] = restart_oss.str();

            std::ostringstream game_oss;

            auto nextPlayer = (i + 1) % numPlayers;
            
            auto continue_it = continue_state.find(nextPlayer);
            if (continue_it == continue_state.end()) {
                game_oss << "setup ";
                writeSetup(game_oss, o);
                game_oss << "\n";
                for (auto &it : moves_since_last) {
                    if (it.first != nextPlayer || numPlayers == 1) {
                        game_oss << it.second << " ";
                    }
                }
                game_oss << "end\n";
            } else {
                game_oss << "move ";
                for (auto &it : moves_since_last) {
                    if (it.first != nextPlayer || numPlayers == 1) {
                        game_oss << it.second << " ";
                    }
                }
                game_oss << "end\n";
                game_oss << continue_it->second << "\n";
            }
            
            continue_game = game_oss.str();
            std::cout << "continue " << continue_game << "\n";
        }
    }

    for (auto i = 0; i < numPlayers; i++) {
        Opening last;
        auto continue_game = continue_state[i];
        std::istringstream last_read("move end\n" + continue_game);
        last_read >> last;
        readEncodedSetup(last_read, last);
        auto score = score_player_map(i, last.setup.weights, last.setup.map);
        std::cout << "player " << i << " score " << score << "\n";
    }
    
    return 0;
}
