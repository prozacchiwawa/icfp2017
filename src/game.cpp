#include "game.h"
#include "plan.h"

Move Opening::run() {
    auto edges = setup.map.getUnclaimedEdges();
    auto neighborhood = NeighborhoodSizeClassifier();
    auto connected = ConnectedToLambda();
    auto classifier = PlusClassifier(neighborhood, connected);
    std::vector<Edge> scores;
    std::transform(edges.begin(), edges.end(), std::back_inserter(scores), [&] (const std::pair<SiteID,SiteID> &p) {
        auto e = Edge(p.first, p.second);
        e.score = classifier.classify(setup.punter, e, setup.map);
        return e;
    });
    std::sort(scores.begin(), scores.end(), Edge::score_greater);
    if (scores.size() > 0) {
        return Move::claim(setup.punter, scores[0].a, scores[0].b);
    } else {
        return Move::pass(setup.punter);
    }
}
