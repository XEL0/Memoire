#include "Graph.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>

#include "randomGenerator.hpp"

Graph::Graph(const unsigned V, std::unique_ptr<std::vector<Edge>> E): V(V), E(std::move(E)) {
    V2 = std::make_unique<std::unordered_set<Vertex>>();
}

Graph::Graph(const unsigned V): V(V) {
    E = std::make_unique<std::vector<Edge>>();
    V2 = std::make_unique<std::unordered_set<Vertex>>();
}

bool Graph::isEmbedded() const { return embedded; }

ComparabilityGraph::ComparabilityGraph(const unsigned V, const unsigned dim, const unsigned ub) : Graph(V), rand(0, ub),
dim(dim){
    this->generateOrdering();
    this->computeEdgesSet();
}

void ComparabilityGraph::generateOrdering() {
    ordering.resize(V);
    for (unsigned i = 0; i < V; i++) {
        for (unsigned j = 0; j < dim; j++) {
            ordering[i].push_back(rand());
            std::cout << ordering[i][j] << " ";
        }
    }
    embedded = true;
}

void ComparabilityGraph::computeEdgesSet() {
    for (unsigned i = 0; i < V; i++) {
        for (unsigned j = 0; j < i; j++) {
            const bool smaller = std::ranges::all_of(
                std::views::zip(ordering[i], ordering[j]),
                [](auto pair) { auto [a, b] = pair; return a < b; });

            if (not isInV1(i)) continue;
            const bool greater = std::ranges::all_of(
                std::views::zip(ordering[i], ordering[j]),
                [](auto pair) { auto [a, b] = pair; return a > b; });

            if (smaller or greater) {
                E->push_back({i, j});
                std::cout << "(" << i << ", " << j << ")" << std::endl;
            }
        }
    }
}

void ComparabilityGraph::computeBipartiteEdgesSet() {
    for (unsigned i = 0; i < V; i++) {
        for (unsigned j = 0; j < i; j++) {
            if (not (isInV1(i) xor isInV1(j))) continue;
            bool smaller;
            if (isInV1(i)) {
                smaller = std::ranges::all_of(
                std::views::zip(ordering[i], ordering[j]),
                [](auto pair) { auto [a, b] = pair; return a < b; });
            } else {
                smaller = std::ranges::all_of(
                std::views::zip(ordering[i], ordering[j]),
                [](auto pair) { auto [a, b] = pair; return a > b; });
            }
            if (smaller) {
                E->push_back({i, j});
                std::cout << "(" << i << ", " << j << ")" << std::endl;
            }
        }
    }

}

void ComparabilityGraph::constructV2(const unsigned q) {
    std::vector<Vertex> v(V);
    std::iota(v.begin(), v.end(), 0);
    std::ranges::shuffle(v, rand.getRNG());
    v.resize(q);
    for (auto v2 : v) {
        V2->insert(v2);
    }
}

ComparabilityGraph::ComparabilityGraph(const unsigned V, const unsigned dim,
    const unsigned p, const unsigned q, const unsigned ub) : Graph(V),
    rand(0, ub), dim(dim) {
    bipartite = true;
    assert(p + q == V);
    this->generateOrdering();
    this->constructV2(q);
    this->computeBipartiteEdgesSet();
}
