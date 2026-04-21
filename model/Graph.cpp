#include "Graph.hpp"

#include <algorithm>
#include <iostream>
#include <ostream>

#include "RandomGenerator.hpp"

Graph::Graph(): V(0), V1(std::make_unique<std::unordered_set<Vertex>>()), E(std::make_unique<std::vector<Edge>>()) {
}

Graph::Graph(std::unique_ptr<std::unordered_set<Vertex>> V, std::unique_ptr<std::vector<Edge> > E) :
    V(0), V1(std::move(V)), E(std::move(E)) {
    this->V = V1->size();
}

void Graph::generate(const unsigned V){
    this->Graph::constructV(V, 0);
    this->Graph::constructEdgesSet();
}

BipartiteGraph::BipartiteGraph(): V2(std::make_unique<std::unordered_set<Vertex>>()) {}

BipartiteGraph::BipartiteGraph(std::unique_ptr<std::unordered_set<Vertex> > V1,
                               std::unique_ptr<std::unordered_set<Vertex> > V2,
                               std::unique_ptr<std::vector<Edge> > E) :
    Graph(std::move(V1), std::move(E)), V2(std::move(V2)) {
    this->V = this->V1->size() + this->V2->size();
}

void BipartiteGraph::generate(const unsigned p, const unsigned q){
    this->BipartiteGraph::constructV(p, q);
    this->BipartiteGraph::constructEdgesSet();
}



ComparabilityGraph::ComparabilityGraph() :
    dim{},
    point_space_limit{},
    ordering(std::make_shared<std::unordered_map<Vertex, std::vector<unsigned>>>())
    {}

ComparabilityGraph::ComparabilityGraph(std::unique_ptr<std::unordered_set<Vertex>> V,
                                       std::unique_ptr<std::vector<Edge>> E,
                                       const std::shared_ptr<std::unordered_map<Vertex, std::vector<unsigned>>> &ordering,
                                       const unsigned dim,
                                       const unsigned point_space_limit) :
    Graph(std::move(V), std::move(E)),
    dim(dim),
    point_space_limit(point_space_limit),
    ordering(ordering)
    {}


void ComparabilityGraph::generate(const unsigned V, const unsigned dim, const unsigned point_space_limit){
    this->dim = dim;
    this->point_space_limit = point_space_limit;
    this->Graph::constructV(V, 0);
    constructOrdering();
    this->Graph::constructEdgesSet();
}




ComparabilityBigraph::ComparabilityBigraph()= default;

ComparabilityBigraph::ComparabilityBigraph(std::unique_ptr<std::unordered_set<Vertex>> V1,
                                           std::unique_ptr<std::unordered_set<Vertex>> V2,
                                           std::unique_ptr<std::vector<Edge>> E,
                                           const std::shared_ptr<std::unordered_map<Vertex, std::vector<unsigned>>> &ordering,
                                           const unsigned dim,
                                           const unsigned point_space_limit):
    BipartiteGraph(std::unique_ptr<std::unordered_set<Vertex>>{},
                  std::move(V2), std::unique_ptr<std::vector<Edge>>{}),
    Graph(std::move(V1), std::move(E)),
    ComparabilityGraph(std::unique_ptr<std::unordered_set<Vertex>>{},
                       std::unique_ptr<std::vector<Edge>>{},
                       ordering, dim, point_space_limit)
{
    bipartite = true;
    embedded = true;
}


void ComparabilityBigraph::generate(const unsigned p, const unsigned q, const unsigned dim, const unsigned point_space_limit){
    this->dim = dim;
    this->point_space_limit = point_space_limit;
    this->BipartiteGraph::constructV(p, q);
    this->constructOrdering();
    this->ComparabilityBigraph::constructEdgesSet();
}








void Graph::constructV(const unsigned p, const unsigned q) {
    this->V = p + q;
    for (unsigned i = 0; i < p; i++) V1->insert(i);
}

void BipartiteGraph::constructV(const unsigned p, const unsigned q) {
    this->V = p + q;
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
    E->insert(E->end(), all_possible_edges->begin(), all_possible_edges->end());
    // if we don't want the graph to be complete
    /*const auto edge_count = all_possible_edges->size();
    auto rg = RandomGenerator(0, edge_count);
    const auto wanted_edges = rg();
    std::ranges::shuffle(*all_possible_edges, rg.getRNG());

    E->insert(E->end(),
              all_possible_edges->begin(),
              all_possible_edges->begin() + wanted_edges);
    for (auto e : edges()) {
        std::cout << "(" << e.first << ", " << e.second << ")" << std::endl;
    }*/
}

void ComparabilityGraph::constructOrdering() {
    auto rg = RandomGenerator(0, point_space_limit);
    for (const Vertex v : vertices()) {
        ordering->operator[](v) = {};
        for (unsigned d = 0; d < dim; d++) {
            ordering->at(v).push_back(rg());
        }
    }
    embedded = true;
}


std::unique_ptr<std::vector<Edge>> Graph::constructAllPossibleEdges() {
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
                std::views::zip(ordering->at(u), ordering->at(v)),
                [](auto pair) { auto [a, b] = pair; return a < b; });

    const bool greater = std::ranges::all_of(
        std::views::zip(ordering->at(u), ordering->at(v)),
        [](auto pair) { auto [a, b] = pair; return a > b; });

    return smaller or greater;
}

bool ComparabilityBigraph::comparable(const Vertex u, const Vertex v) const {
    if (not BipartiteGraph::comparable(u, v)) return false;

    bool smaller;
    if (isInV1(u)) {
        smaller = std::ranges::all_of(
                        std::views::zip(ordering->at(u), ordering->at(v)),
                        [](auto pair) { auto [a, b] = pair; return a < b; });
    } else {
        smaller = std::ranges::all_of(
                std::views::zip(ordering->at(u), ordering->at(v)),
                [](auto pair) { auto [a, b] = pair; return a > b; });
    }
    return smaller;
}

unsigned Graph::size() const {
    return V;
}