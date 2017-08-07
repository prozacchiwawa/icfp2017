#pragma once

#include <set>
#include <string>
#include <deque>
#include <queue>
#include <memory>
#include "types.h"
#include "ourgraph.h"

class Opening;
class DumbMap;

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

class ConnectednessClassifier : public Classifier {
public:
 ConnectednessClassifier(const std::map<SiteID, uint64_t > &b) : branches(b){ }
    virtual double classify(PID us, const Edge &e, const DumbMap &d) const override;
 private:
    const std::map<SiteID, uint64_t > &branches;
};

class BuildPlan {
public:
    virtual std::vector<Edge> recommendMoves() const = 0;
    virtual double scoreWhenComplete() const = 0;
    virtual bool moveEliminates(PID punter, const std::pair<SiteID, SiteID> &move, const Opening &o) const = 0;
    virtual int totalCost() const = 0;
    virtual void addMove(PID punter, const std::pair<SiteID, SiteID> &move, const Opening &o) = 0;

    virtual std::string name() const = 0;
    virtual std::string serialize() const = 0;
};

class DandelionPlan : public BuildPlan {
public:
    DandelionPlan
        (PID punter,
         SiteID v0,
         const SiteID &mine,
         const std::vector<SiteID> &path,
         const Opening &world);
    DandelionPlan
        (const std::string &serialize, const Opening &world);
    
    std::vector<Edge> recommendMoves() const override;
    double scoreWhenComplete() const override;
    bool moveEliminates(PID punter, const std::pair<SiteID, SiteID> &move, const Opening &o) const override;
    int totalCost() const override;
    void addMove(PID punter, const std::pair<SiteID, SiteID> &move, const Opening &o) override;

    std::string name() const override { return "dandelion"; }
    std::string serialize() const override;

private:
    std::set<std::pair<SiteID, SiteID> >
        generateRecommendedMoves(const SiteID &v0, const SiteID &mine, const Opening &o);
    double computeScore(PID punter, SiteID mine, const Graph &player_graph, const Opening &o);

    PID punter;
    std::string mine;
    std::set<std::pair<SiteID, SiteID> > edges;
    int currentCostVal;
    double scoreWhenCompleteVal;
};

std::ostream &operator << (std::ostream &oustr, const DandelionPlan &bp);

class TestPlan : public BuildPlan {
public:
    TestPlan(PID punter);
    TestPlan(const std::string &serialize, const Opening &world);
    
    std::vector<Edge> recommendMoves() const override;
    double scoreWhenComplete() const override;
    bool moveEliminates(PID punter, const std::pair<SiteID, SiteID> &move, const Opening &o) const override;
    int totalCost() const override;
    void addMove(PID punter, const std::pair<SiteID, SiteID> &move, const Opening &o) override;

    std::string name() const override { return "test"; }
    std::string serialize() const override;

private:
    void construct();
    std::set<std::pair<SiteID, SiteID> >
        generateRecommendedMoves(const SiteID &v0, const SiteID &mine, const Opening &o);
    double computeScore(PID punter, SiteID mine, const Graph &player_graph, const Opening &o);

    PID punter;
    int cost_max;
    std::set<std::pair<SiteID, SiteID> > edges;
};

struct GreaterScore {
    bool operator() (const std::shared_ptr<BuildPlan> &a, const std::shared_ptr<BuildPlan> &b) const {
        return a->scoreWhenComplete() > b->scoreWhenComplete();
    }
};

class Planner {
public:
    void initPlans(Opening &op);
    std::shared_ptr<BuildPlan> current() const;
    void addMove(PID punter, const std::string &a, const std::string &b, Opening &o);

    std::istream &read(std::istream &instr, const Opening &o);
    std::ostream &write(std::ostream &oustr, const Opening &o) const;

    std::deque<std::shared_ptr<BuildPlan> > plans;
};
