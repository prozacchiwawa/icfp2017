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
        std::cin >> o.setup.moves;
    } else {
        // Only make a move during the main loop, not during setup
        OurState os(o);
        std::cin >> os;
        auto take_move = o.run();
        o.setup.moves.insert(take_move);
        std::cout << take_move << "\n";

    }

    // Always send state
    std::cout << OurState(o) << "\n";

    return 0;
}
