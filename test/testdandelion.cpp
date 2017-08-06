#include <iostream>
#include <fstream>
#include "ourgraph.h"
#include "game.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "usage: testdandelion map.crk\n";
        return 1;
    }
    std::ifstream gamefile(argv[1]);
    std::string continue_game
        ((std::istreambuf_iterator<char>(gamefile)),
         std::istreambuf_iterator<char>());
    std::string game_state;
    int numMoves = -1;
    std::map<SiteID, std::set<SiteID> > whereToBuild;

    std::istringstream begin_game(continue_game);
    Opening o;
    begin_game >> o;
    o.setupFinalize();
    
    if (o.ot == SetupOp) {
        numMoves = o.setup.map.edge_count;
        for (auto &mine_it : o.setup.map.mines) {
            auto &build_ref = whereToBuild[mine_it];
            o.generateDandelionLine(mine_it, build_ref);
            if (build_ref.size() == 0) {
                std::cout << "No viable dandelions for " << mine_it << "\n";
            } else {
                std::cout << "Dandelions for " << mine_it << "\n";
                for (auto &d_at : build_ref) {
                    std::vector<SiteID> dplan;
                    o.gradientToMine(mine_it, d_at, dplan);
                    std::cout << "gradient to mine " << mine_it << " from " << d_at << "\n";
                    for (auto &gtm : dplan) {
                        std::cout << gtm << "\n";
                    }
                    DandelionPlan dp(0, d_at, mine_it, dplan, o);
                    std::ostringstream oss;
                    oss << dp;
                    std::cout
                        << "Dandelion for " << d_at << "\n"
                        << oss.str() << "\n";
                    DandelionPlan rp(oss.str(), o);
                    std::cout
                        << "Reread dandelion " << d_at << "\n"
                        << rp << "\n";
                }
            }
        }
    }
    return 0;
}
