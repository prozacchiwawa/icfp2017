#include "plan.h"

double ZeroClassifier::classify(PID us, const Edge &e, const DumbMap &d) const {
    return 0;
}

double ConnectedToLambda::classify(PID us, const Edge &e, const DumbMap &d) const {
    double x = 0;
    auto &player_mines = d.player_mines[us];
    if (d.mines.find(e.a) != d.mines.end() &&
        player_mines.find(e.a) == player_mines.end()) {
        x += 1000.0;
    }
    if (d.mines.find(e.b) != d.mines.end() &&
        player_mines.find(e.a) == player_mines.end()) {
        x += 1000.0;
    }

    auto &pv = d.player_vertices[us];

    if ((pv.find(e.a) == pv.end() &&
         pv.find(e.b) != pv.end()) ||
        (pv.find(e.b) == pv.end() &&
         pv.find(e.a) != pv.end())) {
        x += 500.0;
    }
    
    return x;
}

double NeighborhoodSizeClassifier::classify(PID us, const Edge &e, const DumbMap &d) const {
    auto edgesAwayA = d.getEdgesAway(e.a, 50);
    auto edgesAwayB = d.getEdgesAway(e.b, 50);
    return 10.0 * edgesAwayA.size() + edgesAwayB.size();
}

DandelionPlan::DandelionPlan
(PID punter,
 const SiteID &v0,
 const SiteID &mine,
 const std::vector<SiteID> &path,
 const Opening &world) {
    double score;
    int cost;
    int at = 0;

    auto &d = world.setup.map;
    auto &mines = d.player_mines[punter];
    auto &player_vertices = d.player_vertices[punter];
    auto &pg = d.played[punter];

#if 0
    while (at <= path.size()) {
        Graph player_added = pg;
        edges = generateRecommendedMoves();
        for (auto &e : edges) {
            
        }
        score = score_one_mine(mine, 
    }
#endif
}

DandelionPlan::DandelionPlan
(const std::string &serialized, const Opening &world) {
}

std::vector<Edge> DandelionPlan::recommendMoves() const {
    std::vector<Edge> v;
    return v;
}

double DandelionPlan::scoreWhenComplete() const {
    return 0.0;
}

double DandelionPlan::presentScore() const {
    return 0.0;
}

bool DandelionPlan::moveEliminates(PID punter, const std::pair<SiteID, SiteID> &move) const {
    return true;
}

int DandelionPlan::totalCost() const {
    return 0;
}

int DandelionPlan::currentCost() const {
    return 0;
}

std::string DandelionPlan::serialize() const {
    return "";
}
