#include <iostream>
#include "ourgraph.h"
#include "game.h"

int main() {
    OurState s;
    std::cin >> s;
    auto take_move = s.run();
    std::cout << take_move << "\n";
    std::cout << s << "\n";
    return 0;
}
