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

double ConnectednessClassifier::classify(PID us, const Edge &e, const DumbMap &d) const {
  // Note: this assumes that we are building from a to b
  return 10.0 * branches.find(e.b)->second * branches.find(e.b)->second;
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
 const Opening &world) : punter(punter), mine(mine) {
    double best_score = -1;
    double score;
    int cost;
    int at = 0;

    auto &d = world.setup.map;
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
    std::istringstream iss(frombase64(serialized));

    iss >> mine >> currentCostVal >> scoreWhenCompleteVal;
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

bool DandelionPlan::moveEliminates(PID punter, const std::pair<SiteID, SiteID> &move, const Opening &o) const {
    return (punter == o.setup.punter && edges.size() == 1 && edges.find(move) != edges.end()) || (punter != o.setup.punter && edges.find(move) != edges.end());
}

int DandelionPlan::totalCost() const {
    return edges.size();
}

int DandelionPlan::currentCost() const {
    return currentCostVal;
}

std::string DandelionPlan::serialize() const {
    std::ostringstream oss;
    oss
        << mine << " "
        << currentCostVal << " "
        << scoreWhenCompleteVal << "\n";
    for (auto &it : edges) {
        oss << it.first << " " << it.second << "\n";
    }
    oss << "end\n";
    return tobase64(oss.str());
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
    std::string lit_val = v0;
    for (auto &it : pathHome) {
        if (lit_val != "") {
            res.insert(make_ordered_pair(it, lit_val));
        } else {
            res.insert(make_ordered_pair(v0, it));
        }
        lit_val = it;
    }
    if (lit_val != "") {
        res.insert(make_ordered_pair(lit_val, mine));
    }
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
    auto &d = o.setup.map;

    edges.erase(move);
    currentCostVal = 0;
    for (auto &it : edges) {
        if (d.played_edges.find(it) != d.played_edges.end()) {
            currentCostVal++;
        }
    }

    auto orig_graph = d.played[punter];
    auto player_graph = d.played[punter];
    auto &player_vertices = d.player_vertices[punter];
    
    for (auto &e : edges) {
        auto a_it = d.vertices_by_name.find(e.first);
        auto b_it = d.vertices_by_name.find(e.second);
        if (a_it == d.vertices_by_name.end() ||
            b_it == d.vertices_by_name.end()) {
            throw std::exception();
        }
        auto a_v = *a_it;
        auto b_v = *b_it;
        boost::add_edge(a_v.second, b_v.second, 1, player_graph);
    }
}

double DandelionPlan::computeScore(PID punter, SiteID mine, const Graph &player_graph, const Opening &o) {
    auto &d = o.setup.map;
    auto &player_vertices = d.player_vertices[punter];
    auto &orig_graph = d.played[punter];
    return score_player_map(punter, o.setup.weights, player_graph, d) -
        score_player_map(punter, o.setup.weights, orig_graph, d);
}

std::ostream &operator << (std::ostream &oustr, const DandelionPlan &dp) {
    return oustr << dp.serialize();
}

void Planner::initPlans(Opening &o) {
    std::map<SiteID, std::set<SiteID> > whereToBuild;
    for (auto &mine_it : o.setup.map.mines) {
        auto &build_ref = whereToBuild[mine_it];
        o.generateDandelionLine(mine_it, build_ref);
        for (auto &d_at : build_ref) {
            std::vector<SiteID> dplan;
            o.gradientToMine(mine_it, d_at, dplan);
            auto dp = std::make_shared<DandelionPlan>(0, d_at, mine_it, dplan, o);
            plans.push(dp);
        }
    }
}

std::shared_ptr<BuildPlan> Planner::current() const {
    if (plans.empty()) {
        return std::shared_ptr<BuildPlan>();
    } else {
        return plans.top();
    }
}

std::istream &Planner::read(std::istream &instr, const Opening &o) {
    std::string ty;
    instr >> ty;
    while (ty != "end") {
        if (ty == "dandelion") {
            instr >> ty;
            auto dp = std::make_shared<DandelionPlan>(ty, o);
            plans.push(dp);
        } else {
            std::string enc;
            instr >> enc;
            std::cerr << "skipping unknown " << ty << " " << enc << "\n";
            throw std::exception();
        }
        instr >> ty;
    }
    return instr;
}

std::ostream &Planner::write(std::ostream &oustr, const Opening &o) const {
    auto planner_copy = plans;
    while (!planner_copy.empty()) {
        auto p = planner_copy.top();
        planner_copy.pop();
        oustr << p->name() << " " << p->serialize() << "\n";
    }
    oustr << "end\n";
    return oustr;
}

void Planner::addMove(PID punter, const std::string &a, const std::string &b, Opening &o) {
    std::priority_queue<std::shared_ptr<BuildPlan> > q;
    while (!plans.empty()) {
        auto p = plans.top();
        plans.pop();
        auto pair = make_ordered_pair(a,b);
        if (!p->moveEliminates(punter, pair, o)) {
            q.push(p);
        } else if (punter == o.setup.punter) {
            p->addMove(punter, pair, o);
            q.push(p);
        }
    }
    plans = q;
}
