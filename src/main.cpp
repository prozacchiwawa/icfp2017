#include <unistd.h>
#include <signal.h>
#include <memory>
#include <iostream>
#include <exception>
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/topological_sort.hpp"

extern "C" {
#include "base64.h"
}

using PID = int;
using SID = std::string;
using vecS = boost::vecS;
using bidirectionalS = boost::bidirectionalS;
using Weight = int;
using Graph = boost::adjacency_list<vecS, vecS, bidirectionalS, SID, Weight>;
using Vertex = boost::graph_traits < Graph >::vertex_descriptor;

enum OpeningType {
    SetupOp, MoveOp
};

std::istream &operator >> (std::istream &instr, OpeningType &o) {
    std::string r;
    instr >> r;
    if (r == "setup") {
        o = SetupOp;
        return instr;
    } else if (r == "move") {
        o = MoveOp;
        return instr;
    } else {
        throw new std::exception();
    }
}

struct DumbMap {
    Graph g;
};

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
        instr >> r >> s;
        if (r != "end" && s != "end") {
            boost::add_edge(vertices_by_name[r], vertices_by_name[s], 1, m.g);
        } else {
            break;
        };
    }
    return instr;
}

std::ostream &operator << (std::ostream &oustr, const DumbMap &m) {
    return oustr;
}

struct OpeningSetup {
    PID punter;
    size_t punters;
    DumbMap map;
};

std::istream &operator >> (std::istream &instr, OpeningSetup &os) {
    instr >> os.punter;
    instr >> os.punters;
    instr >> os.map;
    return instr;
}

std::ostream &operator << (std::ostream &oustr, const OpeningSetup &os) {
    return oustr << os.punter << " " << os.punters << " " << os.map;
}

enum MoveType {
    Claim, Pass
};

std::istream &operator >> (std::istream &instr, MoveType &m) {
    std::string r;
    instr >> r;
    if (r == "claim") {
        m = Claim;
    } else if (r == "pass") {
        m = Pass;
    } else {
        throw std::exception();
    }
    return instr;
}

struct ClaimMove {
    SID source, target;
};

std::istream &operator >> (std::istream &instr, ClaimMove &m) {
    instr >> m.source;
    instr >> m.target;
}

std::ostream &operator << (std::ostream &oustr, const ClaimMove &m) {
    oustr << m.source << " " << m.target;
}

struct Move {
    MoveType moveType;
    PID punter;
    ClaimMove claim;

    static Move pass(PID n) {
        Move m = { Pass, n };
        return m;
    }
};

std::istream &operator >> (std::istream &instr, Move &m) {
    instr >> m.moveType;
    instr >> m.punter;
    if (m.moveType == Claim) {
        instr >> m.claim;
    }
    return instr;
}

struct Opening {
    OpeningType ot;
    OpeningSetup setup;
    Move move;
};

std::istream &operator >> (std::istream &instr, Opening &o) {
    instr >> o.ot;
    if (o.ot == SetupOp) {
        instr >> o.setup;
    } else {
        instr >> o.move;
    }
    return instr;
}

Move the_move;

std::ostream &operator << (std::ostream &oustr, const Move &m) {
    if (m.moveType == Claim) {
        return oustr << "claim " << " " << m.punter << " " << m.claim.source << m.claim.target << "\n";
    } else {
        return oustr << "pass " << m.punter << "\n";
    }
}

using Moves = std::vector<Move>;

struct OurState {
    OpeningSetup setup;
    Moves moves;
};

std::istream &operator >> (std::istream &instr, Moves &m) {
    while (true) {
        try {
            Move mv;
            instr >> mv;
            m.push_back(mv);
        } catch (...) {
            break;
        }
    }
    return instr;
}

std::istream &operator >> (std::istream &instr, OurState &s) {
    std::string r;
    instr >> r;
    size_t bsize = b64d_size(r.size());
    std::vector<char> vec(bsize);
    b64_decode(reinterpret_cast<const unsigned char*>(r.c_str()), r.size(), reinterpret_cast<unsigned int *>(&vec[0]));
    std::istringstream iss(std::string(&vec[0], bsize));
    return iss >> s.setup;
}

std::ostream &operator << (std::ostream &oustr, const OurState &s) {
    std::ostringstream oss;
    oss << s.setup;
    auto ostr = oss.str();
    size_t bsize = b64e_size(ostr.size());
    std::vector<char> vec(bsize);
    b64_encode(reinterpret_cast<const unsigned int *>(ostr.c_str()), ostr.size(), reinterpret_cast<unsigned char *>(&vec[0]));
    return oustr << std::string(&vec[0], vec.size());
}

int main() {
    OurState s;
    Opening o;
    std::cin >> o;
    if (o.ot == SetupOp) {
        s.setup = o.setup;
    } else {
        std::cin >> s.setup >> s.moves;
    }
    the_move = Move::pass(s.setup.punter);
    std::cout << the_move;
    std::cout << s;
    return 0;
}
