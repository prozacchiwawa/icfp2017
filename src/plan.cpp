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

#if 0
DandelionPlan::DandelionPlan
(const SiteID &v0,
 const SiteID &mine,
 const std::vector<SiteID> &path,
 const Opening &world) {
}

DandelionPlan::DandelionPlan
(const std::string &serialized, const Opening &world) {
}

std::vector<Edge> DandelionPlan::recommendedMoves() const {
}

double DandelionPlan::scoreWhenComplete() const {
}

double DandelionPlan::presentScore() const {
}

bool DandelionPlan::moveEliminates(PID punter, const std::pair<SiteID, SiteID> &move) const {
}

int DandelionPlan::totalCost() const {
}

int DandelionPlan::currentCost() const {
}

std::string DandelionPlan::serialize() const {
}
#endif
