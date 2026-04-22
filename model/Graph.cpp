#include "Graph.hpp"

#include <algorithm>
#include <ostream>

#include "RandomGenerator.hpp"

Graph::Graph() = default;
Graph::Graph(std::vector<VertexPointer> vertices) : vertices(std::move(vertices)) {}

Bigraph::Bigraph() = default;
Bigraph::Bigraph(std::vector<VertexPointer> vertices): Graph(std::move(vertices)) {}

ComparabilityGraph::ComparabilityGraph(): dim(0), point_space_limit(1000) {}
ComparabilityGraph::ComparabilityGraph(std::vector<VertexPointer> vertices, const unsigned dim, const unsigned point_space_limit):
    Graph(std::move(vertices)), dim(dim), point_space_limit(point_space_limit) {}

ComparabilityBigraph::ComparabilityBigraph() = default;
ComparabilityBigraph::ComparabilityBigraph(const std::vector<VertexPointer> &vertices, unsigned dim, unsigned point_space_limit)
    : Graph(vertices), ComparabilityGraph(std::vector<VertexPointer>{}, dim, point_space_limit) {}


void Graph::generate(const unsigned size) {
    this->constructV(size);
}

void Bigraph::generate(unsigned p, unsigned q) {
    this->constructV(p, q);
}

void ComparabilityGraph::generate(const unsigned size, const unsigned dim, const unsigned point_space_limit) {
    this->dim = dim;
    this->point_space_limit = point_space_limit;
    this->Graph::constructV(size);
    constructOrdering();
}

void ComparabilityBigraph::generate(const unsigned p, const unsigned q, const unsigned dim, const unsigned point_space_limit){
    this->dim = dim;
    this->point_space_limit = point_space_limit;
    this->constructV(p, q);
    this->constructOrdering();
}

void Graph::constructV(const unsigned size) {
    vertices.resize(size);
    for (unsigned i = 0; i < size; i++) { vertices[i] = std::make_shared<Vertex>(i); }
}

void Bigraph::constructV(const unsigned p, const unsigned q) {
    const unsigned size = p + q;
    vertices.resize(size);
    auto rg = RandomGenerator(0, size);
    std::vector<unsigned> ids(size);
    std::iota(ids.begin(), ids.end(), 0);
    std::ranges::shuffle(ids, rg.getRNG());
    int i = 0;
    for (auto id : ids) {
        i < p ? vertices[i] = std::make_shared<ColoredVertex>(id, 0) : vertices[i] = std::make_shared<ColoredVertex>(id, 1);
        i++;
    }
}

void ComparabilityGraph::constructV(const unsigned size) {
    vertices.resize(size);
    for (unsigned i = 0; i < size; i++) { vertices[i] = std::make_shared<EmbeddedVertex>(i, std::vector<unsigned>(dim)); }
}

void ComparabilityBigraph::constructV(const unsigned p, const unsigned q) {
    const unsigned size = p + q;
    vertices.resize(size);
    auto rg = RandomGenerator(0, size);
    std::vector<unsigned> ids(size);
    std::iota(ids.begin(), ids.end(), 0);
    std::ranges::shuffle(ids, rg.getRNG());
    int i = 0;
    for (auto id : ids) {
        i < p ? vertices[i] = std::make_shared<ColoredEmbeddedVertex>(id, 0, std::vector<unsigned>(dim)):
                vertices[i] = std::make_shared<ColoredEmbeddedVertex>(id, 1, std::vector<unsigned>(dim));
        i++;
    }
}


void ComparabilityGraph::constructOrdering() {
    auto rg = RandomGenerator(0, point_space_limit);
    for (const auto& v : enumerate()) {
        const auto ev = std::dynamic_pointer_cast<EmbeddedVertex>(v);
        for (unsigned d = 0; d < dim; d++) {
            ev->embed(d, rg());
        }
    }
}

std::unique_ptr<std::vector<Edge>> Graph::makeComplete() {
    std::vector<Edge> all_possible_edges;
    const auto V = size();
    all_possible_edges.reserve(V * (V - 1) / 2);

    for (auto it = vertices.begin(); it != vertices.end(); ++it){
        for (auto jt = std::next(it); jt != vertices.end(); ++jt){
            if (not comparable(*it, *jt)) continue;
            all_possible_edges.emplace_back(*it, *jt);
        }
    }
    return std::make_unique<std::vector<Edge>>(std::move(all_possible_edges));
}

bool Bigraph::comparable(const VertexPointer &u, const VertexPointer &v) const {
    const auto cu = std::dynamic_pointer_cast<ColoredVertex>(u);
    const auto cv = std::dynamic_pointer_cast<ColoredVertex>(v);
    return cu->isComparableTo(*cv);
}

bool ComparabilityGraph::comparable(const VertexPointer &u, const VertexPointer &v) const {
    const auto cu = std::dynamic_pointer_cast<EmbeddedVertex>(u);
    const auto cv = std::dynamic_pointer_cast<EmbeddedVertex>(v);
    return cu->isComparableTo(*cv);
}

bool ComparabilityBigraph::comparable(const VertexPointer &u, const VertexPointer &v) const {
    const auto cu = std::dynamic_pointer_cast<ColoredEmbeddedVertex>(u);
    const auto cv = std::dynamic_pointer_cast<ColoredEmbeddedVertex>(v);
    return cu->isComparableTo(*cv);
}