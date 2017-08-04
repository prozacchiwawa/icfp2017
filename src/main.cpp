#include <unistd.h>
#include <signal.h>
#include <memory>
#include <iostream>
#include <exception>
#include <sstream>

#include "ourgraph.h"

extern "C" {
#include "base64.h"
}

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
    return instr;
}

std::ostream &operator << (std::ostream &oustr, const ClaimMove &m) {
    oustr << m.source << " " << m.target;
    return oustr;
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
        return oustr << "pass " << m.punter;
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

int main() {
    OurState s;
    Opening o;
    std::cin >> o;
    if (o.ot == SetupOp) {
        s.setup = o.setup;
    } else {
        std::cin >> s;
    }
    the_move = Move::pass(s.setup.punter);
    std::cout << the_move << "\n"; 
    std::cout << s << "\n";
    return 0;
}
