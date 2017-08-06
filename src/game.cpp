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

void Opening::generateDandelionLine(SiteID v0, std::set<SiteID> &candidates) {
    auto edge_count = setup.map.edge_count;
    auto max_distance = edge_count / setup.punters;
    auto weight_it = setup.weights.find(v0);
    if (weight_it == setup.weights.end()) {
        std::cerr << "Tried to read vertex " << v0 << " which doesn't exist\n";
        throw std::exception();
    }
    auto &weight_ref = weight_it->second;

    auto max_dist = 0;
    std::string where_max;
    for (int i = 0; i < 2; i++) {
        for (auto &vit : setup.map.vertices_by_name) {
            auto vtx = vit.second;
            if (max_dist < weight_ref[vtx]) {
                max_dist = weight_ref[vtx];
                where_max = vit.first;
            }
            if (weight_ref[vtx] == max_distance) {
                candidates.insert(vit.first);
            }
        }
        if (candidates.size() == 0 && where_max != "") {
            max_distance = max_dist;
        }
    }
}
