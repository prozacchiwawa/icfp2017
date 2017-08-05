#pragma once

#include "game.h"

class Edge {
public:
    std::string a;
    std::string b;
    double score;

    Edge(const std::string &a, const std::string &b) : a(a), b(b), score() { }
    static bool score_greater(const Edge &a, const Edge &b) {
        return a.score > b.score;
    }
};

class Classifier {
public:
    virtual double classify(PID us, const Edge &e, const DumbMap &d) const = 0;
};

class ZeroClassifier : public Classifier {
    virtual double classify(PID us, const Edge &e, const DumbMap &d) const override;
};

template <class A, class B>
class PlusClassifier : public Classifier {
    const A &a;
    const B &b;
public:
    PlusClassifier(const A &a, const B &b) : a(a), b(b) { }
    double classify(PID us, const Edge &e, const DumbMap &d) const override {
        return a.classify(us, e, d) + b.classify(us, e, d);
    }
};

class ConnectedToLambda : public Classifier {
public:
    virtual double classify(PID us, const Edge &e, const DumbMap &d) const override;
};

class NeighborhoodSizeClassifier : public Classifier {
public:
    virtual double classify(PID us, const Edge &e, const DumbMap &d) const override;
};
