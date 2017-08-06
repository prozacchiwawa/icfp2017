#include "plan.h"
#include "game.h"
#include "score_eval.h"

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
 SiteID v0,
 const SiteID &mine,
 const std::vector<SiteID> &path,
 const Opening &world) {
    double best_score = -1;
    double score;
    int cost;
    int at = 0;

    auto &d = world.setup.map;
    auto &mines = d.player_mines[punter];
    auto &pg = d.played[punter];

    while (at <= path.size()) {
        auto player_graph = pg;
        auto new_edges = generateRecommendedMoves(v0, mine, world);
        for (auto &e : new_edges) {
            auto a_it = d.vertices_by_name.find(e.first);
            auto b_it = d.vertices_by_name.find(e.second);
            if (a_it == d.vertices_by_name.end() ||
                b_it == d.vertices_by_name.end()) {
                throw std::exception();
            }
            auto a_v = *a_it;
            auto b_v = *b_it;
            boost::add_edge(a_v.second, b_v.second, player_graph);
        }
        
        score = computeScore(punter, mine, player_graph, world);

        if (best_score == -1 || score > best_score) {
            edges = std::move(new_edges);
            best_score = score;
            scoreWhenCompleteVal = best_score;
        } else if (score < best_score) {
            break;
        }

        if (at < path.size()) {
            v0 = path[at];
        }
        at++;
    }
}

DandelionPlan::DandelionPlan
(const std::string &serialized, const Opening &world) {
    std::string r;
    std::istringstream iss(serialized);
    iss >> r;
    edges.clear();
    while (r != "end") {
        std::string s;
        iss >> s;
        edges.insert(make_ordered_pair(r, s));
        iss >> r;
    }
}

std::vector<Edge> DandelionPlan::recommendMoves() const {
    std::vector<Edge> v;
    std::transform
        (edges.begin(),
         edges.end(),
         std::back_inserter(v),
         [&] (const std::pair<SiteID, SiteID> &e) {
            return Edge(e.first, e.second);
        });
    return v;
}

double DandelionPlan::scoreWhenComplete() const {
    return scoreWhenCompleteVal;
}

double DandelionPlan::presentScore() const {
    return currentScoreVal;
}

bool DandelionPlan::moveEliminates(PID punter, const std::pair<SiteID, SiteID> &move, const Opening &o) const {
    return punter != o.setup.punter && edges.find(move) != edges.end();
}

int DandelionPlan::totalCost() const {
    return edges.size();
}

int DandelionPlan::currentCost() const {
    return currentCostVal;
}

std::string DandelionPlan::serialize() const {
    std::ostringstream oss;
    for (auto &it : edges) {
        oss << it.first << " " << it.second << "\n";
    }
    oss << "end\n";
    return oss.str();
}

std::set<std::pair<SiteID, SiteID> > DandelionPlan::generateRecommendedMoves(const SiteID &v0, const SiteID &mine, const Opening &o) {
    std::vector<SiteID> pathHome;
    std::set<std::pair<SiteID, SiteID> > res;
    auto vtx_it = o.setup.map.vertices_by_name.find(v0);
    if (vtx_it == o.setup.map.vertices_by_name.end()) {
        return res;
    }
    auto vtx = vtx_it->second;
    o.gradientToMine(mine, v0, pathHome);
    std::string lit_val;
    for (auto &it : pathHome) {
        if (lit_val != "") {
            res.insert(make_ordered_pair(it, lit_val));
        } else {
            res.insert(make_ordered_pair(v0, it));
        }
        lit_val = it;
    }
    res.insert(make_ordered_pair(lit_val, mine));
    for (auto ep = boost::in_edges(vtx, o.setup.map.world);
         ep.first != ep.second;
         ++ep.first) {
        auto vs1 = boost::source(*ep.first, o.setup.map.world);
        auto vs2 = boost::target(*ep.first, o.setup.map.world);
        auto vs1_it = o.setup.map.vertices_by_number.find(vs1);
        auto vs2_it = o.setup.map.vertices_by_number.find(vs2);
        if (vs1_it == o.setup.map.vertices_by_number.end() ||
            vs2_it == o.setup.map.vertices_by_number.end()) {
            throw std::exception();
        }
        auto &vs1_name = vs1_it->second;
        auto &vs2_name = vs2_it->second;
        res.insert(make_ordered_pair(vs1_name, vs2_name));
    }
    for (auto ep = boost::out_edges(vtx, o.setup.map.world);
         ep.first != ep.second;
         ++ep.first) {
        auto vs1 = boost::source(*ep.first, o.setup.map.world);
        auto vs2 = boost::target(*ep.first, o.setup.map.world);
        auto vs1_it = o.setup.map.vertices_by_number.find(vs1);
        auto vs2_it = o.setup.map.vertices_by_number.find(vs2);
        if (vs1_it == o.setup.map.vertices_by_number.end() ||
            vs2_it == o.setup.map.vertices_by_number.end()) {
            throw std::exception();
        }
        auto &vs1_name = vs1_it->second;
        auto &vs2_name = vs2_it->second;
        res.insert(make_ordered_pair(vs1_name, vs2_name));
    }
    return res;
}

void DandelionPlan::addMove(PID punter, const std::pair<SiteID, SiteID> &move, const Opening &o) {
    edges.erase(move);
    currentScoreVal = 0;
    for (auto &it : edges) {
        if (o.setup.map.played_edges.find(it) != o.setup.map.played_edges.end()) {
            currentScoreVal++;
        }
    }
}

double DandelionPlan::computeScore(PID punter, SiteID mine, const Graph &player_graph, const Opening &o) {
    auto &d = o.setup.map;
    auto &player_vertices = d.player_vertices[punter];
    return score_one_mine
        (mine, player_vertices, o.setup.weights, player_graph, d);
}
