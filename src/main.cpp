#include <unistd.h>
#include <signal.h>
#include <memory>
#include <iostream>
#include <exception>
#include <sstream>

#include "ourgraph.h"
#include "game.h"

extern "C" {
#include "base64.h"
}

Opening o;

void termination_handler(int signum) {
    Opening q;
    
    std::cerr << "Caught " << signum << std::endl;
    if (signum != SIGUSR1) {
        exit(1);
    }
    q.setup.punter = o.setup.punter;
    q.setup.punters = o.setup.punters;
    q.setup.map = o.setup.map;
    q.setup.moves = o.setup.moves;
    if (o.ot != SetupOp) {
        auto take_move = o.run(true);
        if (take_move.moveType == Claim) {
            o.addMove
                (take_move.punter,
                 take_move.claimMove.source,
                 take_move.claimMove.target);
        }
        std::cout << take_move << "\n";
    }

    std::cerr << "punter " << q.setup.punter << " punters " << q.setup.punters << "\n";
    writeEncodedSetup(std::cout, q);
    std::cout << "\n";
    exit(0);
}

int main() {
    // Set a sigusr1 handler
    struct sigaction new_action, old_action;

    new_action.sa_handler = termination_handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;
    
    sigaction (SIGUSR1, &new_action, &old_action);
    
    std::cin >> o;
    if (o.ot == SetupOp) {
        o.setupFinalize();
        //sleep(11); // stall here to test signal handler
    } else {
        // Only make a move during the main loop, not during setup
        readEncodedSetup(std::cin, o);
        auto take_move = o.run();
        if (take_move.moveType == Claim) {
            o.addMove
                (take_move.punter,
                 take_move.claimMove.source,
                 take_move.claimMove.target);
        }
        //sleep(2); // stall here to test signal handler
        std::cout << take_move << "\n";
    }

    // Always send state
    writeEncodedSetup(std::cout, o);
    std::cout << "\n";

    return 0;
}
