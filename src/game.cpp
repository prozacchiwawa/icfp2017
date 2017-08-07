#include "game.h"
#include "plan.h"
#include <boost/graph/dijkstra_shortest_paths.hpp>

Move Opening::run() {
    auto edges = setup.map.getUnclaimedEdges();
    auto neighborhood = NeighborhoodSizeClassifier();
    auto connected = ConnectedToLambda();
    auto classifier = PlusClassifier<NeighborhoodSizeClassifier,ConnectedToLambda>(neighborhood, connected);

    // Run plans
    auto plan = setup.planner.current();
    std::vector<Edge> scores;
    if (plan) {
        std::cerr << "have-plan " << plan->scoreWhenComplete() << " " << plan->name() << " " << plan->serialize() << "\n";
        
    }
    auto queue_copy = setup.planner.plans;
    while (!queue_copy.empty()) {
        auto e = queue_copy.top();
        queue_copy.pop();
        std::cerr << "plan " << e->scoreWhenComplete() << " " << e->name() << " " << e->serialize() << "\n";
    }
    
    auto suggested = plan ? plan->recommendMoves() : std::vector<Edge>();
    std::transform(suggested.begin(), suggested.end(), std::back_inserter(scores), [&] (const Edge &e) {
        auto f = e;
        f.score = 10000.0 + classifier.classify(setup.punter, e, setup.map);
        return f;
    });
    
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

void Opening::addMove(PID punter, const std::string &a, const std::string &b) {
    setup.map.addMove(punter, a, b);
    setup.planner.addMove(punter, a, b, *this);
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

    setup.planner.initPlans(*this);

    for (auto &vtx_it : setup.map.vertices_by_name) {
        auto vtx = vtx_it.second;
        Graph::in_edge_iterator ie, ie_end;
        Graph::out_edge_iterator oe, oe_end;
        boost::tie (oe,oe_end) = boost::out_edges(vtx, setup.map.world);
        boost::tie (ie,ie_end) = boost::in_edges(vtx, setup.map.world);
        std::cerr << "Vertex " << setup.map.vertices_by_number[vtx] << ": ";
        uint64_t num_edges = 0;
        for (; ie != ie_end; ie++) {
          auto s = setup.map.vertices_by_number[source(*ie, setup.map.world)];
          auto t = setup.map.vertices_by_number[target(*ie, setup.map.world)];
          std::cerr << "(" << s << "," << t << ") ";
          num_edges++;
        }
        for (; oe != oe_end; oe++) {
          auto s = setup.map.vertices_by_number[source(*oe, setup.map.world)];
          auto t = setup.map.vertices_by_number[target(*oe, setup.map.world)];
          std::cerr << "(" << s << "," << t << ") ";
          num_edges++;
        }
        std::cerr << "[" << num_edges << " edges]" << std::endl;
        setup.branches[vtx_it.first] = num_edges;
    }
}

void Opening::generateDandelionLine(SiteID v0, std::set<SiteID> &candidates) const {
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
            if (weight_ref[vtx] &&
                (weight_ref[vtx] == max_distance ||
                 setup.map.mines.find(vit.first) != setup.map.mines.end())) {
                candidates.insert(vit.first);
            }
        }
        if (candidates.size() == 0 && where_max != "") {
            max_distance = max_dist;
        }
    }
}

void Opening::gradientToMine(SiteID v0, SiteID from, std::vector<SiteID> &line) const {
    auto vtx = setup.map.vtx_idx(from);
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
                vtx = vs1;
            } else if (weight_ref[vs2] < dist) {
                dist = weight_ref[vs2];
                vtx = vs2;
            }
        }
        for (auto ep = boost::out_edges(vtx, setup.map.world);
             ep.first != ep.second;
             ++ep.first) {
            auto vs1 = boost::source(*ep.first, setup.map.world);
            auto vs2 = boost::target(*ep.first, setup.map.world);
            if (weight_ref[vs1] < dist) {
                dist = weight_ref[vs1];
                vtx = vs1;
            } else if (weight_ref[vs2] < dist) {
                dist = weight_ref[vs2];
                vtx = vs2;
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

// NOTE: Don't add local serialized state here, add it to readEncodedSetup
std::istream &readSetup(std::istream &instr, Opening &o) {
    instr >> o.setup.punter;
    instr >> o.setup.punters;
    instr >> o.setup.map;
    instr >> o.setup.moves;
    
    o.setup.map.setPunters(o.setup.punters);
    for (auto &it : o.setup.moves) {
        if (it.moveType == Claim) {
            o.setup.map.addMove(it.punter, it.claimMove.source, it.claimMove.target);
        }
    }
    
    return instr;
}

// NOTE: Don't add local serialized state here, add it to writeEncodedSetup
std::ostream &writeSetup(std::ostream &oustr, const Opening &o) {
    oustr << o.setup.punter << " " << o.setup.punters << " " << o.setup.map << " " << o.setup.moves << "\n";
    return oustr;
}
