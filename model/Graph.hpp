#ifndef MEMOIRE_GRAPH_HPP
#define MEMOIRE_GRAPH_HPP

#include <cassert>
#include <iostream>
#include <memory>
#include <ostream>
#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using Vertex = unsigned;
using Edge = std::pair<Vertex, Vertex>;

class ComparabilityBigraph {
    friend std::ostream& operator<<(std::ostream& os, const ComparabilityBigraph& g);
    friend class BicliquePartitioner;
protected:
    unsigned V;
    unsigned dim;
    unsigned point_space_limit;
    std::unique_ptr<std::unordered_set<Vertex>> V1;
    std::unique_ptr<std::unordered_set<Vertex>> V2;
    std::shared_ptr<std::unordered_map<Vertex, std::vector<unsigned>>> ordering;

    std::unique_ptr<std::vector<Edge>> E;
    bool embedded = false;

    virtual std::unique_ptr<std::vector<Edge>> constructAllPossibleEdges();
    virtual void constructEdgesSet();
    void constructOrdering();
    virtual void constructV(unsigned p, unsigned q);
    [[nodiscard]] virtual bool comparable(Vertex u, Vertex v) const;

public:
    ComparabilityBigraph();
    ComparabilityBigraph(std::unique_ptr<std::unordered_set<Vertex>> V1,
                         std::unique_ptr<std::unordered_set<Vertex>> V2,
                         const std::shared_ptr<std::unordered_map<Vertex, std::vector<unsigned>>> &ordering,
                         unsigned dim,
                         unsigned point_space_limit);
    virtual ~ComparabilityBigraph() = default;

    void generate(unsigned p, unsigned q, unsigned dim, unsigned point_space_limit);

    [[nodiscard]] virtual std::vector<Vertex> vertices() const {
        auto a = std::vector<Vertex>();
        a.reserve(V);
        for (const auto v: *V1) { a.push_back(v); }
        for (const auto v: *V2) { a.push_back(v); }
        return a;
    }

    [[nodiscard]] auto enumerateV1() const {
        return std::views::all(*V1);
    }

    [[nodiscard]] auto enumerateV2() const {
        return std::views::all(*V2);
    }

    [[nodiscard]] bool isInV1(const Vertex v) const {
        return V1->contains(v);
    }

    [[nodiscard]] bool isInV2(const Vertex v) const {
        return V2->contains(v);
    }

    [[nodiscard]] auto edges() const {
        return std::views::all(*E);
    }

    [[nodiscard]] virtual bool isEmbedded() const { return embedded; }
    [[nodiscard]] unsigned size() const { return V; }

    [[nodiscard]] virtual unsigned getDim() const { return dim; }
    [[nodiscard]] virtual const std::vector<unsigned>& getEmbedding(Vertex v) const { return ordering->at(v); }

    [[nodiscard]] virtual unsigned getPointSpaceLimit() const { return point_space_limit; }
};

inline std::ostream& operator<<(std::ostream& os, const ComparabilityBigraph& g) {
    os << "V1 : (";
    for (const auto v : *g.V1) { os << v << ", "; }
    os << "), V2: (";
    for (const auto v : *g.V2) { os << v << ", "; }
    os << ")";
    return os;
}


#endif //MEMOIRE_GRAPH_HPP