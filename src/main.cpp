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
    OurState s;
    std::cin >> o;
    if (o.ot == SetupOp) {
        s.setup = o.setup;
    } else {
        std::cin >> s;
    }
    auto take_move = s.run();
    std::cout << take_move << "\n";
    std::cout << s << "\n";
    return 0;
}
