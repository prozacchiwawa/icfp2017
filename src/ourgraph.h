#pragma once

#include <iostream>
#include <typeinfo>
#include <map>
#include <string>
#include <set>
#include <vector>

#include <boost/variant/get.hpp>
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/topological_sort.hpp"
#include <boost/graph/labeled_graph.hpp>

using PID = int; // punter id
using SiteID = std::string;
using vecS = boost::vecS;
using bidirectionalS = boost::bidirectionalS;
using VertexStoreType = boost::property<boost::vertex_name_t, SiteID>;
using Weight = boost::property<boost::edge_weight_t, int>;

// This definition uses vertex_descriptors (int) to index vertices

using Graph = boost::adjacency_list<vecS, vecS, bidirectionalS, VertexStoreType, Weight>;
using Vertex = boost::graph_traits < Graph >::vertex_descriptor;

struct DumbMap {
    Graph world;

    std::vector<Graph> played;
    std::set<SiteID> mines;
    
    std::vector<std::set<SiteID> > player_mines;
    std::set<std::pair<SiteID, SiteID> > played_edges;

    std::map<SiteID, Vertex> vertices_by_name;
    std::map<int, SiteID> vertices_by_number;

    std::vector<std::pair<SiteID, SiteID> > getEdges() const {
        std::vector<std::pair<SiteID, SiteID> > edges;
        for (auto ep = boost::edges(world); ep.first != ep.second; ++ep.first) {
            auto vs1 = boost::source(*ep.first, world);
            auto vs2 = boost::target(*ep.first, world);
            auto vs1_by_idx_it = vertices_by_number.find(vs1);
            auto vs2_by_idx_it = vertices_by_number.find(vs2);
            if (vs1_by_idx_it == vertices_by_number.end() ||
                vs2_by_idx_it == vertices_by_number.end()) {
                throw std::exception();
            }
            auto vs1_by_idx = *vs1_by_idx_it;
            auto vs2_by_idx = *vs2_by_idx_it;
            if (vs1_by_idx.second < vs2_by_idx.second) {
                edges.push_back(std::make_pair(vs1_by_idx.second, vs2_by_idx.second));
            } else {
                edges.push_back(std::make_pair(vs2_by_idx.second, vs1_by_idx.second));
            }                
        }
        return edges;
    }

    void setPunters(int all) {
        player_mines.resize(all);
        played.resize(all);
    }

    void addMove(PID punter, const std::string &a, const std::string &b) {
        auto a_it = vertices_by_name.find(a);
        auto b_it = vertices_by_name.find(b);
        if (a_it == vertices_by_name.end() || b_it == vertices_by_name.end()) {
            throw std::exception();
        }
        auto a_v = *a_it;
        auto b_v = *b_it;
        auto &playerg = played[punter];
        if (a < b) {
            played_edges.insert(std::make_pair(a, b));
            boost::add_edge(a_v.second, b_v.second, 1, played[punter]);
        } else {
            played_edges.insert(std::make_pair(b, a));
            boost::add_edge(b_v.second, a_v.second, 1, played[punter]);
        }
    }
};

namespace {
std::istream &operator >> (std::istream &instr, DumbMap &m) {
    std::string r, s;
    size_t v = 0, e = 0;
    while (true) {
        instr >> r;
        if (r != "end") {
            auto v = boost::add_vertex(r, m.world);
            m.vertices_by_number[v] = r;
            m.vertices_by_name[r] = v;
        } else {
            break;
        }
    }
    while (true) {
        instr >> r;
        if (r != "end") {
            instr >> s;
            boost::add_edge(m.vertices_by_name[r], m.vertices_by_name[s], 1, m.world);
        } else {
            break;
        };
    }
    // Read in mines
    while(true) {
        instr >> r;
        if (r != "end") {
            m.mines.insert(r);
        } else {
            break;
        }
    }
    return instr;
}

std::ostream &operator << (std::ostream &oustr, const DumbMap &m) {
    boost::graph_traits<Graph>::vertex_iterator v, v_end;
    for (boost::tie(v, v_end) = boost::vertices(m.world); v != v_end; ++v) {
        oustr << *v << " ";
    }
    oustr << "\nend\n";
    for (boost::tie(v, v_end) = boost::vertices(m.world); v != v_end; ++v) {
        boost::graph_traits<Graph>::out_edge_iterator e, e_end;
        for (boost::tie(e, e_end) = boost::out_edges(*v, m.world); e != e_end; ++e) {
            oustr << boost::source(*e, m.world) << " " << boost::target(*e, m.world) << "\n";
        }
    }
    oustr << "end\n";
    return oustr;
}
}
