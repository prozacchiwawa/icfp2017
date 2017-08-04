#pragma once

#include <typeinfo>
#include <iostream>
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/topological_sort.hpp"

using PID = int;
using SID = int;
using vecS = boost::vecS;
using bidirectionalS = boost::bidirectionalS;
using Weight = int;
using Graph = boost::adjacency_list<vecS, vecS, bidirectionalS, SID, Weight>;
using Vertex = boost::graph_traits < Graph >::vertex_descriptor;

struct DumbMap {
    Graph g;

    std::vector<std::pair<SID, SID> > getEdges() const {
        std::vector<std::pair<SID, SID> > edges;
        boost::graph_traits<Graph>::vertex_iterator v, v_end;
        for (boost::tie(v, v_end) = boost::vertices(g); v != v_end; ++v) {
            boost::graph_traits<Graph>::out_edge_iterator e, e_end;
            for (boost::tie(e, e_end) = boost::out_edges(*v, g); e != e_end; ++e) {
                auto vs1 = boost::source(*e, g);
                auto vs2 = boost::target(*e, g);
                edges.push_back(std::make_pair(vs1, vs2));
            }
        }
        return edges;
    }
};

namespace {
std::istream &operator >> (std::istream &instr, DumbMap &m) {
    std::string r, s;
    size_t v = 0, e = 0;
    std::map<SID, Vertex> vertices_by_name;
    while (true) {
        instr >> r;
        if (r != "end") {
            std::istringstream iss(r);
            int n;
            iss >> n;
            auto v = boost::add_vertex(n, m.g);
            vertices_by_name[n] = v;
        } else {
            break;
        }
    }
    while (true) {
        instr >> r;
        if (r != "end") {
            std::istringstream iss_r(r);
            int nn;
            iss_r >> nn;
            instr >> s;
            int mm;
            std::istringstream iss_s(s);
            int s;
            iss_s >> mm;
            boost::add_edge(vertices_by_name[nn], vertices_by_name[mm], 1, m.g);
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
            oustr << boost::source(*e, m.g) << " " << boost::target(*e, m.g) << "\n";
        }
    }
    oustr << "end\n";
    return oustr;
}
}
