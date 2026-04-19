#ifndef MEMOIRE_GRAPH_HPP
#define MEMOIRE_GRAPH_HPP

#include <memory>
#include <ranges>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <vector>

using Vertex = unsigned;
struct EmbeddedVertex {
    Vertex v;
    std::vector<unsigned> dimensions;
};
using Edge = std::pair<Vertex, Vertex>;

class Graph {
protected:
    std::unique_ptr<std::unordered_set<Vertex>> V1;
    std::unique_ptr<std::vector<Edge>> E;

    [[nodiscard]] virtual std::unique_ptr<std::unordered_set<Vertex>> V() const;
    bool bipartite = false;
    bool embedded = false;
    virtual std::unique_ptr<std::vector<Edge>> constructAllPossibleEdges();
    virtual void constructEdgesSet();
    virtual void constructV(unsigned p, unsigned q);
    [[nodiscard]] virtual bool comparable(Vertex u, Vertex v) const;
    Graph() = default;
public:
    Graph(std::unique_ptr<std::unordered_set<Vertex>> V, std::unique_ptr<std::vector<Edge>> E);
    Graph(unsigned V);
    virtual ~Graph() = default;

    [[nodiscard]] virtual std::ranges::iota_view<std::__hash_const_iterator<std::__hash_node<unsigned, void *> *>, std::__hash_const_iterator<std::__hash_node<unsigned, void *> *>>
    vertices() const {
        return std::views::iota(V1->begin(), V1->end());
    }
    [[nodiscard]] auto edges() const {
        return std::views::all(*E);
    }

    [[nodiscard]] virtual bool isEmbedded() const { return embedded; }
    [[nodiscard]] virtual bool isBipartite() const { return bipartite; }
};



class BipartiteGraph : virtual public Graph {
protected:
    std::unique_ptr<std::unordered_set<Vertex>> V2;
    [[nodiscard]] std::unique_ptr<std::unordered_set<Vertex>> V() const override;
    void constructV(unsigned p, unsigned q) override;
    [[nodiscard]] bool comparable(Vertex u, Vertex v) const override;
    BipartiteGraph() = default;
public:
    BipartiteGraph(std::unique_ptr<std::unordered_set<Vertex>> V1,
                   std::unique_ptr<std::unordered_set<Vertex>> V2,
                   std::unique_ptr<std::vector<Edge>> E);
    BipartiteGraph(unsigned p, unsigned q);
    ~BipartiteGraph() override = default;


    [[nodiscard]] virtual bool isInV1(const Vertex v) const {
        return V1->contains(v);
    }
    [[nodiscard]] virtual bool isInV2(const Vertex v) const {
        return V2->contains(v);
    }
    [[nodiscard]] std::ranges::iota_view<std::__hash_const_iterator<std::__hash_node<unsigned, void *> *>, std::__hash_const_iterator<std::__hash_node<unsigned, void *> *>>
    vertices() const override {
        const auto v = V();
        return std::views::iota(v->begin(), v->end());
    }
};



class ComparabilityGraph : virtual public Graph {
    friend class BicliquePartitioner;
protected:
    unsigned dim{};
    unsigned point_space_limit{};
    std::unique_ptr<std::unordered_map<Vertex, std::vector<unsigned>>> FA_ordering;
    std::unique_ptr<std::vector<EmbeddedVertex>> ordering;

    [[nodiscard]] bool comparable(Vertex u, Vertex v) const override;

    void constructOrdering();
    ComparabilityGraph() = default;
public:
    ComparabilityGraph(std::unique_ptr<std::unordered_set<Vertex>> V,
                       std::unique_ptr<std::vector<Edge>> E,
                       std::unique_ptr<std::unordered_map<Vertex, std::vector<unsigned>>> ordering,
                       unsigned dim,
                       unsigned point_space_limit);
    ComparabilityGraph(unsigned V, unsigned dim, unsigned point_space_limit);
    ~ComparabilityGraph() override = default;


    [[nodiscard]] auto embedding() const {
        return std::views::all(*FA_ordering);
    }

    [[nodiscard]] virtual unsigned getPointSpaceLimit() const { return point_space_limit; }
};



class ComparabilityBigraph : virtual public BipartiteGraph, virtual public ComparabilityGraph {
protected:
    [[nodiscard]] bool comparable(Vertex u, Vertex v) const override;
    ComparabilityBigraph() = default;
public:
    ComparabilityBigraph(std::unique_ptr<std::unordered_set<Vertex>> V1,
                         std::unique_ptr<std::unordered_set<Vertex>> V2,
                         std::unique_ptr<std::vector<Edge>> E,
                         std::unique_ptr<std::unordered_map<Vertex, std::vector<unsigned>>> ordering,
                         unsigned dim,
                         unsigned point_space_limit);



    ComparabilityBigraph(unsigned p, unsigned q, unsigned dim, unsigned point_space_limit);
    ~ComparabilityBigraph() override = default;

};


class ComputeOrientedComparabilityBigraph {
    friend class BicliquePartitioner;
};

#endif //MEMOIRE_GRAPH_HPP