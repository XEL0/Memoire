#include "Graph.hpp"

#include <algorithm>
#include <cassert>

#include "RandomGenerator.hpp"

Graph::Graph(std::unique_ptr<std::unordered_set<Vertex>> V, std::unique_ptr<std::vector<Edge> > E) :
    V1(std::move(V)), E(std::move(E)) {}


Graph::Graph(const unsigned V)
    : V1(std::make_unique<std::unordered_set<Vertex>>()),
      E(std::make_unique<std::vector<Edge>>()) {
    this->Graph::constructV(V, 0);
    this->Graph::constructEdgesSet();
}


BipartiteGraph::BipartiteGraph(std::unique_ptr<std::unordered_set<Vertex> > V1,
                               std::unique_ptr<std::unordered_set<Vertex> > V2,
                               std::unique_ptr<std::vector<Edge> > E) :
    Graph(std::move(V1), std::move(E)), V2(std::move(V2)){}


BipartiteGraph::BipartiteGraph(unsigned p, unsigned q)
    : Graph(),
      V2(std::make_unique<std::unordered_set<Vertex>>()) {
    V1 = std::make_unique<std::unordered_set<Vertex>>();
    E = std::make_unique<std::vector<Edge>>();
    this->BipartiteGraph::constructV(p, q);
    this->BipartiteGraph::constructEdgesSet();
}


ComparabilityGraph::ComparabilityGraph(std::unique_ptr<std::unordered_set<Vertex>> V,
                                       std::unique_ptr<std::vector<Edge>> E,
                                       std::unique_ptr<std::unordered_map<Vertex, std::vector<unsigned>>> ordering,
                                       const unsigned dim,
                                       const unsigned point_space_limit) :
    Graph(std::move(V), std::move(E)),
    point_space_limit(point_space_limit),
    dim(dim),
    FA_ordering(std::move(ordering)) {}


ComparabilityGraph::ComparabilityGraph(unsigned V, unsigned dim, unsigned point_space_limit)
    : Graph(),
      dim(dim),
      point_space_limit(point_space_limit),
      FA_ordering(std::make_unique<std::unordered_map<Vertex, std::vector<unsigned>>>()),
      ordering(std::make_unique<std::vector<EmbeddedVertex>>()) {
    V1 = std::make_unique<std::unordered_set<Vertex>>();
    E = std::make_unique<std::vector<Edge>>();
    this->Graph::constructV(V, 0);
    constructOrdering();
    this->Graph::constructEdgesSet();
}


ComparabilityBigraph::ComparabilityBigraph(std::unique_ptr<std::unordered_set<Vertex>> V1,
                                           std::unique_ptr<std::unordered_set<Vertex>> V2,
                                           std::unique_ptr<std::vector<Edge>> E,
                                           std::unique_ptr<std::unordered_map<Vertex, std::vector<unsigned>>> ordering,
                                           const unsigned dim,
                                           const unsigned point_space_limit)
    : Graph(std::move(V1), std::move(E)),
      BipartiteGraph(std::unique_ptr<std::unordered_set<Vertex>>{}, std::move(V2), std::unique_ptr<std::vector<Edge>>{}),
      ComparabilityGraph(std::unique_ptr<std::unordered_set<Vertex>>{}, std::unique_ptr<std::vector<Edge>>{}, std::move(ordering), dim, point_space_limit)
{
    bipartite = true;
    embedded = true;
}


ComparabilityBigraph::ComparabilityBigraph(unsigned p, unsigned q, unsigned dim, unsigned point_space_limit): Graph(), BipartiteGraph(p, q), ComparabilityGraph(p + q, dim, point_space_limit) {

}

void Graph::constructV(const unsigned p, const unsigned q) {
    for (unsigned i = 0; i < p; i++) V1->insert(i);
}

void BipartiteGraph::constructV(const unsigned p, const unsigned q) {
    const unsigned V = p + q;
    auto rg = RandomGenerator(0, V);
    std::vector<Vertex> vertices(V);
    std::iota(vertices.begin(), vertices.end(), 0);
    std::ranges::shuffle(vertices, rg.getRNG());
    int i = 0;
    for (auto v : vertices) {
        i < p ? V1->insert(v) : V2->insert(v);
        i++;
    }
}

void Graph::constructEdgesSet() {
    const std::unique_ptr<std::vector<Edge>> all_possible_edges = constructAllPossibleEdges();
    const auto edge_count = all_possible_edges->size();
    auto rg = RandomGenerator(0, edge_count);
    const auto wanted_edges = rg();
    std::ranges::shuffle(*all_possible_edges, rg.getRNG());

    E->insert(E->end(),
              all_possible_edges->begin(),
              all_possible_edges->begin() + wanted_edges);
}

void ComparabilityGraph::constructOrdering() {
    auto rg = RandomGenerator(0, point_space_limit);
    auto all = V();
    for (const Vertex v : *all) {
        FA_ordering->operator[](v) = {};
        ordering->push_back({v, {}});
    }
    embedded = true;
}


std::unique_ptr<std::unordered_set<Vertex>> Graph::V() const {
    auto V = std::make_unique<std::unordered_set<Vertex>>();
    V->insert(V1->begin(), V1->end());
    return V;
}

std::unique_ptr<std::unordered_set<Vertex>> BipartiteGraph::V() const {
    auto V = Graph::V();
    V->insert(V2->begin(), V2->end());
    return V;
}

std::unique_ptr<std::vector<Edge>> Graph::constructAllPossibleEdges() {
    const size_t V = V1->size();
    std::vector<Edge> all_possible_edges;
    all_possible_edges.reserve(V * (V - 1) / 2);

    for (unsigned i = 0; i < V; ++i) {
        for (unsigned j = i + 1; j < V; ++j) {
            if (not comparable(i, j)) continue;
            all_possible_edges.emplace_back(i, j);
        }
    }
    return std::make_unique<std::vector<Edge>>(std::move(all_possible_edges));

}

bool Graph::comparable(const Vertex u, const Vertex v) const {
    return true;
}

bool BipartiteGraph::comparable(const Vertex u, const Vertex v) const {
    return isInV1(u) xor isInV1(v);
}

bool ComparabilityGraph::comparable(const Vertex u, const Vertex v) const {
    const bool smaller = std::ranges::all_of(
                std::views::zip(FA_ordering->at(u), FA_ordering->at(v)),
                [](auto pair) { auto [a, b] = pair; return a < b; });

    const bool greater = std::ranges::all_of(
        std::views::zip(FA_ordering->at(u), FA_ordering->at(v)),
        [](auto pair) { auto [a, b] = pair; return a > b; });

    return smaller or greater;
}

bool ComparabilityBigraph::comparable(const Vertex u, const Vertex v) const {
    if (not BipartiteGraph::comparable(u, v)) return false;

    bool smaller;
    if (isInV1(u)) {
        smaller = std::ranges::all_of(
                        std::views::zip(FA_ordering->at(u), FA_ordering->at(v)),
                        [](auto pair) { auto [a, b] = pair; return a < b; });
    } else {
        smaller = std::ranges::all_of(
                std::views::zip(FA_ordering->at(u), FA_ordering->at(v)),
                [](auto pair) { auto [a, b] = pair; return a > b; });
    }
    return smaller;
}