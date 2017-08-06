#pragma once

#include <set>
#include <random>
#include <chrono>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "types.h"
#include "ourgraph.h"
#include "plan.h"

extern "C" {
    #include "base64.h"
}

struct ClaimMove {
    SiteID source, target;

    std::pair<SiteID,SiteID> pair() const { return std::make_pair(source, target); }
    
    bool operator < (const ClaimMove &other) const {
        return pair() < other.pair();
    }
};

struct Move {
    MoveType moveType;
    PID punter;
    ClaimMove claimMove;

    std::tuple<MoveType,PID,ClaimMove> tuple() const {
        return std::make_tuple(moveType,punter,claimMove);
    }
    
    bool operator < (const Move &other) const {
        return tuple() < other.tuple();
    }

    static Move pass(PID n) {
        Move m = { Pass, n };
        return m;
    }

    static Move claim(PID n, SiteID a, SiteID b) {
        Move m = { Claim, n, { a, b } };
        return m;
    }
};

using Moves = std::set<Move>;

struct OpeningSetup {
    PID punter;
    size_t punters;
    DumbMap map;
    Moves moves;
    Planner planner;
    std::map<SiteID, std::vector<uint32_t> > weights;
    std::map<SiteID, uint64_t > branches; // number of incident edges
};

struct Opening {
    OpeningType ot;
    OpeningSetup setup;

    Move run();
    void setupFinalize();
    void addMove(PID punter, const std::string &a, const std::string &b);

    void generateDandelionLine(SiteID mine, std::set<SiteID> &vertices) const;
    void gradientToMine(SiteID v0, SiteID from, std::vector<SiteID> &line) const;
};

struct OurState {
    OurState(Opening &o) : setup(o.setup) { }
    OpeningSetup &setup;
};

std::istream &readSetup(std::istream &instr, Opening &o);
std::ostream &writeSetup(std::ostream &oustr, const Opening &os);

namespace {
std::istream &operator >> (std::istream &instr, std::map<std::string, std::vector<uint32_t> > &weights) {
    std::string label;
    instr >> label;
    while (label != "end") {
        int size;
        char c;
        auto &ref = weights[label];
        instr.get(c);
        instr.read(reinterpret_cast<char*>(&size), sizeof(size));
        ref.resize(size / sizeof(uint32_t));
        instr.read(reinterpret_cast<char*>(&ref[0]), size);
        instr >> label;
    }
    return instr;
}

std::ostream &operator << (std::ostream &oustr, const std::map<std::string, std::vector<uint32_t> > &weights) {
    for (auto &it : weights) {
        auto &ref = it.second;
        int size = sizeof(uint32_t) * ref.size();
        oustr << it.first << " ";
        oustr.write(reinterpret_cast<const char*>(&size), sizeof(size));
        oustr.write(reinterpret_cast<const char*>(&(it.second[0])), size);
    }
    oustr << "end\n";
    return oustr;
}

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

std::istream &operator >> (std::istream &instr, ClaimMove &m) {
    instr >> m.source;
    instr >> m.target;
    return instr;
}

std::ostream &operator << (std::ostream &oustr, const ClaimMove &m) {
    oustr << m.source << " " << m.target;
    return oustr;
}

std::istream &operator >> (std::istream &instr, Move &m) {
    instr >> m.moveType;
    instr >> m.punter;
    if (m.moveType == Claim) {
        instr >> m.claimMove;
    }
    return instr;
}

std::ostream &operator << (std::ostream &oustr, const Move &m) {
    if (m.moveType == Claim) {
        return oustr << "claim " << " " << m.punter << " " << m.claimMove << "\n";
    } else {
        return oustr << "pass " << m.punter;
    }
}

std::istream &operator >> (std::istream &instr, Moves &m) {
    while (true) {
        try {
            Move mv;
            instr >> mv;
            m.insert(mv);
        } catch (...) {
            break;
        }
    }
    return instr;
}

std::ostream &operator << (std::ostream &oustr, const Moves &m) {
    for (auto it : m) {
        oustr << it << "\n";
    }
    oustr << " end\n";
    return oustr;
}

std::istream &operator >> (std::istream &instr, Opening &o) {
    instr >> o.ot;
    if (o.ot == SetupOp) {
        readSetup(instr, o);
    } else {
        instr >> o.setup.moves;
    }
    return instr;
}

std::string frombase64(const std::string &r) {
    size_t bsize = b64d_size(r.size());
    std::vector<char> vec(bsize+1);
    b64_decode(r.c_str(), r.size(), &vec[0]);
    return std::string(&vec[0], bsize);
}

// Local precomputation should follow readSetup
std::istream &readEncodedSetup(std::istream &instr, Opening &s) {
    std::string r;
    instr >> r;
    if (r.size() == 0) {
        return instr;
    }
    auto str = frombase64(r);
    std::istringstream iss(str);
    readSetup(iss, s);
    iss >> s.setup.weights;
    s.setup.planner.read(iss, s);
    return instr;
}

std::string tobase64(const std::string &ostr) {
    size_t bsize = b64e_size(ostr.size());
    std::vector<char> vec(bsize+1);
    b64_encode(ostr.c_str(), ostr.size(), &vec[0]);
    return std::string(&vec[0], vec.size()-1);
}

// Local precomputation should follow writeSetup
std::ostream &writeEncodedSetup(std::ostream &oustr, const Opening &o) {
    std::ostringstream oss;
    writeSetup(oss, o);
    oss << o.setup.weights;
    o.setup.planner.write(oss, o);
    auto ostr = oss.str();
    return oustr << tobase64(ostr);
}

Move randomTurn(const Opening &s) {
    std::vector<std::pair<SiteID,SiteID> > can_use;
    std::vector<std::pair<SiteID,SiteID> > edges = s.setup.map.getEdges();

    for (auto &it : edges) {
        auto pe = s.setup.map.played_edges.find(it);
        auto res = pe == s.setup.map.played_edges.end();
        if (res) {
            can_use.push_back(it);
        }
    }
    
    // obtain a time-based seed:
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    
    shuffle (can_use.begin(), can_use.end(), std::default_random_engine(seed));
    if (can_use.begin() != can_use.end()) {
        return Move::claim(s.setup.punter, can_use[0].first, can_use[0].second);
    } else {
        return Move::pass(s.setup.punter);
    }
}
}

void generateMineWeights
    (std::string mine,
     std::map<std::string, std::vector<uint32_t> > &weights,
     const DumbMap &map);
