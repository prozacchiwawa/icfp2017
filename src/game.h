#pragma once

#include <set>
#include <random>
#include <chrono>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "ourgraph.h"
#include "plan.h"

extern "C" {
    #include "base64.h"
}

enum OpeningType {
    SetupOp, MoveOp
};

enum MoveType {
    Claim, Pass
};

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
};

struct Opening {
    OpeningType ot;
    OpeningSetup setup;

    Move run();
};

struct OurState {
    OurState(Opening &o) : setup(o.setup) { }
    OpeningSetup &setup;
};

namespace {
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

std::istream &operator >> (std::istream &instr, OpeningSetup &os) {
    instr >> os.punter;
    instr >> os.punters;
    instr >> os.map;
    instr >> os.moves;
    
    os.map.setPunters(os.punters);
    for (auto &it : os.moves) {
        if (it.moveType == Claim) {
            os.map.addMove(it.punter, it.claimMove.source, it.claimMove.target);
        }
    }
    return instr;
}

std::ostream &operator << (std::ostream &oustr, const OpeningSetup &os) {
    return oustr << os.punter << " " << os.punters << " " << os.map << " " << os.moves;
}

std::istream &operator >> (std::istream &instr, Opening &o) {
    instr >> o.ot;
    if (o.ot == SetupOp) {
        instr >> o.setup;
    } else {
        instr >> o.setup.moves;
    }
    return instr;
}

std::istream &operator >> (std::istream &instr, OurState &s) {
    std::string r;
    instr >> r;
    size_t bsize = b64d_size(r.size());
    std::vector<char> vec(bsize+1);
    b64_decode(r.c_str(), r.size(), &vec[0]);
    std::istringstream iss(std::string(&vec[0], bsize-1));
    return iss >> s.setup;
}

std::ostream &operator << (std::ostream &oustr, const OurState &s) {
    std::ostringstream oss;
    oss << s.setup;
    auto ostr = oss.str();
    size_t bsize = b64e_size(ostr.size());
    std::vector<char> vec(bsize+1);
    b64_encode(ostr.c_str(), ostr.size(), &vec[0]);
    return oustr << std::string(&vec[0], vec.size()-1);
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
