#include "Graph.hpp"

#include <algorithm>
#include <ostream>

#include "RandomGenerator.hpp"

Graph::Graph() = default;
Graph::Graph(std::vector<VertexPointer> vertices, std::vector<Edge> edges) : vertices(std::move(vertices)), edges(std::move(edges)) {}

Bigraph::Bigraph() = default;
Bigraph::Bigraph(std::vector<VertexPointer> vertices, const unsigned p, const unsigned q, std::vector<Edge> edges):
    Graph(std::move(vertices), std::move(edges)), p(p), q(q) {}

ComparabilityGraph::ComparabilityGraph(): dim(0), point_space_limit(1000) {}
ComparabilityGraph::ComparabilityGraph(
    std::vector<VertexPointer> vertices, const unsigned dim, const unsigned point_space_limit, std::vector<Edge> edges):
    Graph(std::move(vertices), std::move(edges)), dim(dim), point_space_limit(point_space_limit) {}

ComparabilityBigraph::ComparabilityBigraph() = default;
ComparabilityBigraph::ComparabilityBigraph(
    std::vector<VertexPointer> vertices, const unsigned p, const unsigned q,
    const unsigned dim, unsigned point_space_limit, std::vector<Edge> edges):
        Bigraph(std::vector<VertexPointer>{}, p, q),
        Graph(std::move(vertices), std::move(edges)),
        ComparabilityGraph(std::vector<VertexPointer>{}, dim, point_space_limit) {}


void Graph::generate(const unsigned size) {
    this->constructV(size);
}

void Bigraph::generate(unsigned p, unsigned q) {
    this->p = p;
    this->q = q;
    this->constructV(p, q);
}

void ComparabilityGraph::generate(const unsigned size, const unsigned dim, const unsigned point_space_limit) {
    this->dim = dim;
    this->point_space_limit = point_space_limit;
    this->constructV(size);
    constructOrdering();
}

void ComparabilityBigraph::generate(const unsigned p, const unsigned q, const unsigned dim, const unsigned point_space_limit){
    this->dim = dim;
    this->point_space_limit = point_space_limit;
    this->p = p;
    this->q = q;
    this->constructV(p, q);
    this->constructOrdering();
}

void Graph::constructV(const unsigned size) {
    vertices.reserve(size);
    for (unsigned i = 0; i < size; i++) { vertices.push_back(std::make_shared<Vertex>(i)); }
}

void Bigraph::constructV(const unsigned p, const unsigned q) {
    const unsigned size = p + q;
    vertices.reserve(size);
    auto rg = RandomGenerator(0, size);
    std::vector<unsigned> ids(size);
    std::iota(ids.begin(), ids.end(), 0);
    std::ranges::shuffle(ids, rg.getRNG());
    int i = 0;
    for (auto id : ids) {
        int color = (i < p) ? 0 : 1;
        vertices.push_back(std::make_shared<ColoredVertex>(id, color));
        i++;
    }
}

void ComparabilityGraph::constructV(const unsigned size) {
    vertices.reserve(size);
    for (unsigned i = 0; i < size; i++) { vertices.push_back(std::make_shared<EmbeddedVertex>(i, std::vector<unsigned>(dim))); }
}

void ComparabilityBigraph::constructV(const unsigned p, const unsigned q) {
    const unsigned size = p + q;
    vertices.reserve(size);
    auto rg = RandomGenerator(0, size);
    std::vector<unsigned> ids(size);
    std::iota(ids.begin(), ids.end(), 0);
    std::ranges::shuffle(ids, rg.getRNG());
    int i = 0;
    for (auto id : ids) {
        int color = (i < p) ? 0 : 1;
        vertices.push_back(std::make_shared<ColoredEmbeddedVertex>(id, color, std::vector<unsigned>(dim)));
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

std::vector<Edge> Graph::makeComplete() {
    std::vector<Edge> all_possible_edges;
    //const auto V = size();
    //all_possible_edges.reserve(V * (V - 1) / 2);

    for (auto it = vertices.begin(); it != vertices.end(); ++it){
        for (auto jt = std::next(it); jt != vertices.end(); ++jt){
            if (not comparable(*it, *jt)) continue;
            all_possible_edges.emplace_back(*it, *jt);
        }
    }
    return all_possible_edges;
}

void Graph::constructE(const bool complete) {
    if (complete) {
        for (const auto& e : makeComplete()) {
            edges.emplace_back(e);
        }
    } else {
        auto all = makeComplete();
        auto rg = RandomGenerator(0, all.size());
        std::ranges::shuffle(all, rg.getRNG());
        const auto nb = rg();
        for (unsigned i = 0; i < nb; i++) {
            edges.emplace_back(all[i]);
        }
    }
}

bool ComparabilityBigraph::isComplete() {
    for (auto it = vertices.begin(); it != vertices.end(); ++it) {
        for (auto jt = std::next(it); jt != vertices.end(); ++jt) {
            if (Bigraph::comparable(*it,*jt) and not comparable(*it, *jt)) return false;
        }
    }
    return true;
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