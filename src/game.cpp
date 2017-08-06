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
            if (weight_ref[vtx] == max_distance ||
                setup.map.mines.find(vit.first) != setup.map.mines.end()) {
                candidates.insert(vit.first);
            }
        }
        if (candidates.size() == 0 && where_max != "") {
            max_distance = max_dist;
        }
    }
}

void Opening::gradientToMine(SiteID v0, SiteID from, std::vector<SiteID> &line) {
    auto vtx_it = setup.map.vertices_by_name.find(from);
    if (vtx_it == setup.map.vertices_by_name.end()) {
        return;
    }
    auto vtx = vtx_it->second;
    auto weight_it = setup.weights.find(v0);
    if (weight_it == setup.weights.end()) {
        std::cerr << "Tried to read vertex " << v0 << " which doesn't exist\n";
        throw std::exception();
    }
    auto &weight_ref = weight_it->second;
    auto dist = weight_ref[vtx];

    while (dist > 0 && dist != std::numeric_limits<int>::max()) {
        auto started_with = vtx;
        for (auto ep = boost::in_edges(vtx, setup.map.world);
             ep.first != ep.second;
             ++ep.first) {
            auto vs1 = boost::source(*ep.first, setup.map.world);
            auto vs2 = boost::target(*ep.first, setup.map.world);
            if (weight_ref[vs1] < dist) {
                dist = weight_ref[vs1];
                vtx = weight_ref[vs1];
            } else if (weight_ref[vs2] < dist) {
                dist = weight_ref[vs2];
                vtx = weight_ref[vs2];
            }
        }
        for (auto ep = boost::out_edges(vtx, setup.map.world);
             ep.first != ep.second;
             ++ep.first) {
            auto vs1 = boost::source(*ep.first, setup.map.world);
            auto vs2 = boost::target(*ep.first, setup.map.world);
            if (weight_ref[vs1] < dist) {
                dist = weight_ref[vs1];
                vtx = weight_ref[vs1];
            } else if (weight_ref[vs2] < dist) {
                dist = weight_ref[vs2];
                vtx = weight_ref[vs2];
            }
        }
        if (vtx == started_with) {
            // Couldn't follow :-(
            line.clear();
            break;
        } else {
            auto out_it = setup.map.vertices_by_number.find(vtx);
            if (out_it == setup.map.vertices_by_number.end()) {
                throw std::exception();
            }
            auto &vtx_name = out_it->second;
            line.push_back(vtx_name);
        }
    }
}
