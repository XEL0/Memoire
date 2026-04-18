#ifndef MEMOIRE_GRAPH_HPP
#define MEMOIRE_GRAPH_HPP
#include <memory>
#include <ranges>
#include <unordered_set>
#include <utility>
#include <vector>

#include "randomGenerator.hpp"

using Vertex = unsigned;
using Edge = std::pair<Vertex, Vertex>;
struct EmbeddedVertex {
    Vertex v;
    std::vector<unsigned> dimensions;
};

class Graph {
protected:
    Vertex V;
    std::unique_ptr<std::unordered_set<Vertex>> V2;
    std::unique_ptr<std::vector<Edge>> E;
    bool bipartite = false;
    bool embedded = false;
public:

    Graph(unsigned V, std::unique_ptr<std::vector<Edge>> E);
    explicit Graph(unsigned V);
    virtual ~Graph() = default;

    virtual bool isEmbedded() const;

    auto vertices() const {
        return std::views::iota(Vertex{0}, V);
    }

    auto edges() const {
        return std::views::all(*E);
    }

    virtual bool isInV2(const Vertex v) const {
        return V2->contains(v);
    }

    virtual bool isInV1(const Vertex v) const {
        return not V2->contains(v);
    }

    virtual bool isBipartite() const { return bipartite; }

};


class ComparabilityGraph : public Graph {
private:
    RandomGenerator rand;
protected:
    const unsigned dim;
    std::vector<EmbeddedVertex> ordering;

    void generateOrdering();
    void computeEdgesSet();
    void computeBipartiteEdgesSet();
    void constructV2(unsigned q);

public:
    ComparabilityGraph(unsigned V, unsigned dim, unsigned ub);
    ComparabilityGraph(unsigned V, unsigned dim, unsigned p, unsigned q, unsigned ub);
    ~ComparabilityGraph() override = default;

    auto embedding() const {
        return std::views::all(ordering);
    }
};

#endif //MEMOIRE_GRAPH_HPP