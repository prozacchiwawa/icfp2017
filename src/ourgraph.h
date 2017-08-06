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
#include <boost/graph/kruskal_min_spanning_tree.hpp>
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

template <class A>
std::pair<A,A> make_ordered_pair(const A &a, const A &b) {
    if (a < b) { return std::make_pair(a, b); }
    else { return std::make_pair(b, a); }
}

struct DumbMap {
    Graph world;

    std::vector<Graph> played;
    std::set<SiteID> mines;
    
    std::vector<std::set<SiteID> > player_mines;
    std::set<std::pair<SiteID, SiteID> > played_edges;
    std::vector<std::set<SiteID> > player_vertices;

    std::map<SiteID, Vertex> vertices_by_name;
    std::map<int, SiteID> vertices_by_number;

    int edge_count;

    DumbMap() : edge_count() { }

    template <class E>
    void with_edge(const E &ep, std::function<void(const std::string &a, const std::string &b)> f) const {
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
        f(vs1_by_idx.second, vs2_by_idx.second);
    }
    
    std::vector<std::pair<SiteID, SiteID> > getEdgesAway(const std::string &from, int limit = -1) const {
        std::vector<std::pair<SiteID, SiteID> > res;
        auto vtx_it = vertices_by_name.find(from);
        if (vtx_it == vertices_by_name.end()) {
            return res;
        }
        auto vtx = vtx_it->second;
        for (auto ep = boost::in_edges(vtx, world);
             ep.first != ep.second;
             ++ep.first) {
            if (limit >= 0 && res.size() >= limit) {
                return res;
            }
            with_edge(ep, [&] (const std::string &a, const std::string &b) {
                    res.push_back(make_ordered_pair(a,b));
            });
        }
        
        for (auto ep = boost::out_edges(vtx, world);
             ep.first != ep.second;
             ++ep.first) {
            if (limit >= 0 && res.size() >= limit) {
                return res;
            }
            with_edge(ep, [&] (const std::string &a, const std::string &b) {
                    res.push_back(make_ordered_pair(a,b));
            });
        }
        return res;
    }
    
    std::vector<std::pair<SiteID, SiteID> > getEdges() const {
        std::vector<std::pair<SiteID, SiteID> > edges;
        for (auto ep = boost::edges(world); ep.first != ep.second; ++ep.first) {
            with_edge(ep, [&] (const std::string &a, const std::string &b) {
                    edges.push_back(make_ordered_pair(a,b));
            });
        }
        return edges;
    }

    std::vector<std::pair<SiteID, SiteID> > getUnclaimedEdges() const {
        std::vector<std::pair<SiteID, SiteID> > edges;
        for (auto ep = boost::edges(world); ep.first != ep.second; ++ep.first) {
            with_edge(ep, [&] (const std::string &a, const std::string &b) {
                    auto p = make_ordered_pair(a,b);
                    if (played_edges.find(p) == played_edges.end()) {
                        edges.push_back(p);
                    }
            });
        }
        return edges;
    }

    void setPunters(int all) {
        player_mines.resize(all);
        player_vertices.resize(all);
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
        if (mines.find(a) != mines.end()) {
            player_mines[punter].insert(a);
        }
        if (mines.find(b) != mines.end()) {
            player_mines[punter].insert(b);
        }
        player_vertices[punter].insert(a);
        player_vertices[punter].insert(b);
        auto op =
            make_ordered_pair(std::make_pair(a, a_v), std::make_pair(b, b_v));
        auto ap = &op.first;
        auto bp = &op.second;
        played_edges.insert(std::make_pair(ap->first, bp->first));
        boost::add_edge(ap->second.second, bp->second.second, 1, played[punter]);
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
            m.edge_count++;
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
        auto vtx_it = m.vertices_by_number.find(*v);
        if (vtx_it == m.vertices_by_number.end()) {
            throw std::exception();
        }
        auto vtx = *vtx_it;
        oustr << vtx.second << " ";
    }
    oustr << "\nend\n";
    for (auto ep = boost::edges(m.world); ep.first != ep.second; ++ep.first) {
        m.with_edge(ep, [&] (const std::string &a, const std::string &b) {
            auto p = make_ordered_pair(a,b);
            oustr << p.first << " " << p.second << "\n";
        });
    }
    oustr << "end\n";
    for (auto &it : m.mines) {
        oustr << it << " ";
    }
    oustr << "end\n";
    
    return oustr;
}
}
