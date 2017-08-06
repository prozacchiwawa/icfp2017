#include "game.h"
#include "plan.h"
#include <boost/graph/dijkstra_shortest_paths.hpp>

Move Opening::run() {
    auto edges = setup.map.getUnclaimedEdges();
    auto neighborhood = NeighborhoodSizeClassifier();
    auto connected = ConnectedToLambda();
    auto classifier = PlusClassifier<NeighborhoodSizeClassifier,ConnectedToLambda>(neighborhood, connected);
    
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

void generateMineWeights
    (std::string mine,
     std::map<std::string, std::vector<uint32_t> > &weights,
     const DumbMap &map) {
    auto v0_idx_it = map.vertices_by_name.find(mine);
    if (v0_idx_it == map.vertices_by_name.end()) {
        throw std::exception();
    }
    auto v0_idx = v0_idx_it->second;
    auto &world_distances = weights[mine];
    std::cerr << "map-size " << map.vertices_by_name.size() << "\n";
    world_distances.assign
        (map.vertices_by_name.size(), std::numeric_limits<int>::max());
    boost::dijkstra_shortest_paths
        (map.world, v0_idx, boost::distance_map(&world_distances[0]));
}

void Opening::setupFinalize() {
    for (auto &it : setup.map.mines) {
        auto v0 = it;
        generateMineWeights(v0, setup.weights, setup.map);
    }
}
