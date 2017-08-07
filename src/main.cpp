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

int main() {
    Opening o;
    std::cin >> o;
    if (o.ot == SetupOp) {
        o.setupFinalize();
    } else {
        // Only make a move during the main loop, not during setup
        readEncodedSetup(std::cin, o);
        auto take_move = o.run();
        if (take_move.moveType == Claim) {
            o.addMove(take_move.punter, take_move.claimMove.source, take_move.claimMove.target);
        }
        std::cout << take_move << "\n";

    }

    // Always send state
    writeEncodedSetup(std::cout, o);
    std::cout << "\n";

    return 0;
}
