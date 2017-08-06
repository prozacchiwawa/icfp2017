#pragma once

#include <queue>
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

class BuildPlan {
public:
    virtual std::vector<Edge> recommendMoves() const = 0;
    virtual double scoreWhenComplete() const = 0;
    virtual double presentScore() const = 0;
    virtual bool moveEliminates(PID punter, const std::pair<SiteID, SiteID> &move) const = 0;
    virtual int totalCost() const = 0;
    virtual int currentCost() const = 0;

    virtual std::string serialize() const = 0;

    bool operator < (const BuildPlan &other) const {
        auto cost_ratio = scoreWhenComplete() / (double)currentCost();
        auto other_cost_ratio = other.scoreWhenComplete() / (double)other.currentCost();
        return cost_ratio > other_cost_ratio;
    }
};

#if 0
class DandelionPlan : public BuildPlan {
public:
    DandelionPlan
        (const SiteID &v0,
         const SiteID &mine,
         const std::vector<SiteID> &path,
         const Opening &world);
    DandelionPlan
        (const std::string &serialize, const Opening &world);
    
    std::vector<Edge> recommendMoves() const override;
    double scoreWhenComplete() const override;
    double presentScore() const override;
    bool moveEliminates(PID punter, const std::pair<SiteID, SiteID> &move) const override;
    int totalCost() const override;
    int currentCost() const override;

    std::string serialize() const override;

private:
    std::vector<std::pair<SiteID, SiteID> > edges;
};

class Planner {
public:
    void initPlans(Opening &op);
    std::priority_queue<BuildPlan> plans;
};
#endif
