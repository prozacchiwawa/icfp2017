#pragma once

#include <iostream>
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/topological_sort.hpp"

using PID = int;
using SID = std::string;
using vecS = boost::vecS;
using bidirectionalS = boost::bidirectionalS;
using Weight = int;
using Graph = boost::adjacency_list<vecS, vecS, bidirectionalS, SID, Weight>;
using Vertex = boost::graph_traits < Graph >::vertex_descriptor;

struct DumbMap {
    Graph g;
};

namespace {
std::istream &operator >> (std::istream &instr, DumbMap &m) {
    std::string r, s;
    size_t v = 0, e = 0;
    std::map<SID, Vertex> vertices_by_name;
    while (true) {
        instr >> r;
        if (r != "end") {
            auto v = boost::add_vertex(r, m.g);
            vertices_by_name[r] = v;
        } else {
            break;
        }
    }
    while (true) {
        instr >> r;
        if (r != "end") {
            instr >> s;
            boost::add_edge(vertices_by_name[r], vertices_by_name[s], 1, m.g);
        } else {
            break;
        };
    }
    return instr;
}

std::ostream &operator << (std::ostream &oustr, const DumbMap &m) {
    boost::graph_traits<Graph>::vertex_iterator v, v_end;
    for (boost::tie(v, v_end) = boost::vertices(m.g); v != v_end; ++v) {
        oustr << *v << " ";
    }
    oustr << "\nend\n";
    for (boost::tie(v, v_end) = boost::vertices(m.g); v != v_end; ++v) {
        boost::graph_traits<Graph>::out_edge_iterator e, e_end;
        for (boost::tie(e, e_end) = boost::out_edges(*v, m.g); e != e_end; ++e) {
            
        }
    }
    oustr << "\nend\n";
    return oustr;
}
}
