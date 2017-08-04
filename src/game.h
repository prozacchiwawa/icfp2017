#include <set>
#include <random>
#include <chrono>
#include <iostream>
#include <algorithm>

#include "ourgraph.h"

extern "C" {
    #include "base64.h"
}

enum OpeningType {
    SetupOp, MoveOp
};

struct OpeningSetup {
    PID punter;
    size_t punters;
    DumbMap map;
};

enum MoveType {
    Claim, Pass
};

struct ClaimMove {
    SID source, target;

    std::pair<SID,SID> pair() const { return std::make_pair(source, target); }
    
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

    static Move claim(PID n, SID a, SID b) {
        Move m = { Claim, n, { a, b } };
        return m;
    }
};

struct Opening {
    OpeningType ot;
    OpeningSetup setup;
    Move move;
};

using Moves = std::vector<Move>;

struct OurState {
    OpeningSetup setup;
    Moves moves;
    std::set<std::pair<SID,SID> > edges;    

    Move run();
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

std::istream &operator >> (std::istream &instr, OpeningSetup &os) {
    instr >> os.punter;
    instr >> os.punters;
    instr >> os.map;
    return instr;
}

std::ostream &operator << (std::ostream &oustr, const OpeningSetup &os) {
    return oustr << os.punter << " " << os.punters << " " << os.map;
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
}

std::ostream &operator << (std::ostream &oustr, const ClaimMove &m) {
    oustr << m.source << " " << m.target;
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

std::istream &operator >> (std::istream &instr, Opening &o) {
    instr >> o.ot;
    if (o.ot == SetupOp) {
        instr >> o.setup;
    } else {
        instr >> o.move;
    }
    return instr;
}

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

std::ostream &operator << (std::ostream &oustr, Moves &m) {
    for (auto it : m) {
        oustr << it << "\n";
    }
    return oustr;
}

std::istream &operator >> (std::istream &instr, OurState &s) {
    std::string r;
    instr >> r;
    size_t bsize = b64d_size(r.size());
    std::vector<char> vec(bsize);
    b64_decode(r.c_str(), r.size(), &vec[0]);
    std::istringstream iss(std::string(&vec[0], bsize));
    return iss >> s.setup;
}

std::ostream &operator << (std::ostream &oustr, const OurState &s) {
    std::ostringstream oss;
    oss << s.setup;
    auto ostr = oss.str();
    size_t bsize = b64e_size(ostr.size());
    std::vector<char> vec(bsize);
    b64_encode(ostr.c_str(), ostr.size(), &vec[0]);
    return oustr << std::string(&vec[0], vec.size());
}

Move randomTurn(const OurState &s) {
    std::vector<std::pair<SID,SID> > can_use;
    std::vector<std::pair<SID,SID> > edges = s.setup.map.getEdges();

    std::cout << "randomTurn from " << edges.size() << "\n";
    for (auto it : edges) {
        std::cout << "t " << it.first << "," << it.second << "\n";
    }
  
    std::copy_if(edges.begin(), edges.end(), can_use.begin(), [&] (std::pair<SID,SID> &edge) {
            auto found = s.edges.find(edge);
            return found != s.edges.end();
        });
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

Move OurState::run() {
    return randomTurn(*this);
}
