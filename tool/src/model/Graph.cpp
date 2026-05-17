#include "Graph.hpp"

#include <algorithm>
#include <ostream>

#include "RandomGenerator.hpp"

Graph::Graph() = default;
Graph::Graph(std::vector<VertexPointer> vertices, std::vector<Edge> edges) : vertices(std::move(vertices)), edges(std::move(edges)) {}

Bigraph::Bigraph() = default;
Bigraph::Bigraph(std::vector<VertexPointer> vertices, const unsigned p, const unsigned q, std::vector<Edge> edges):
    Graph(std::move(vertices), std::move(edges)), p(p), q(q) {}

ComparabilityGraph::ComparabilityGraph(): dim(0), point_space_limit(-1) {}
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

Terrain::Terrain() : point_space_limit(-1) {}
Terrain::Terrain(std::vector<VertexPointer> vertices, const unsigned point_space_limit, std::vector<Edge> edges)
    : Graph(std::move(vertices), std::move(edges)), point_space_limit(point_space_limit) {}

TerrainVisibilityGraph::TerrainVisibilityGraph() = default;
TerrainVisibilityGraph::TerrainVisibilityGraph(std::vector<VertexPointer> vertices, std::vector<Edge> edges)
    : Graph(std::move(vertices), std::move(edges)) {}

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

void Terrain::generate(const unsigned n, const unsigned point_space_limit) {
    this->point_space_limit = point_space_limit;
    constructV(n);
    constructOrdering();
    constructE(true);
}

void TerrainVisibilityGraph::generateFromTerrain(const Terrain& terrain) {
    this->constructV(terrain.size());
    std::vector<VertexPointer> V = {};
    for (const auto& v : terrain.enumerate()) {V.push_back(v);}
    std::vector<std::pair<VertexPointer, VertexPointer>> terrain_edges = {};
    for (const auto& e : terrain.enumerate_edges()) {
        edges.push_back(e);
        terrain_edges.push_back(e);
    }

    for (auto it = V.begin(); it != V.end(); ++it) {
        for (auto jt = std::next(it); jt != V.end(); ++jt) {
            const auto& u = *it;
            const auto& v = *jt;

            if (v->getId() == u->getId() + 1) continue;
            bool visible = true;

            const unsigned ax = terrain.getEmbeddingAt(u, 0);
            const unsigned ay = terrain.getEmbeddingAt(u, 1);
            const unsigned zx = terrain.getEmbeddingAt(v, 0);
            const unsigned zy = terrain.getEmbeddingAt(v, 1);

            for (auto kt = std::next(it); kt != jt; ++kt) {
                const auto& p = *kt;
                const unsigned px = terrain.getEmbeddingAt(p, 0);
                const unsigned py = terrain.getEmbeddingAt(p, 1);

                if (orientation(ax, ay, px, py, zx, zy) < 0) {
                    visible = false;
                    break;
                }
            }
            if (visible) edges.emplace_back(u, v);
        }
    }
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

void Terrain::constructV(const unsigned n) {
    vertices.reserve(n);
    for (unsigned i = 0; i < n; i++) { vertices.push_back(std::make_shared<EmbeddedVertex>(i, std::vector<unsigned>(2))); }
}

void ComparabilityGraph::constructOrdering() {
    auto rg = RandomGenerator(0, point_space_limit);
    std::vector<unsigned> values(point_space_limit+1);
    std::iota(values.begin(), values.end(), 0);

    for (unsigned d = 0; d < dim; d++) {
        std::ranges::shuffle(values, rg.getRNG());
        for (unsigned i = 0; i < vertices.size(); i++) {
            const auto ev = std::dynamic_pointer_cast<EmbeddedVertex>(vertices[i]);
            ev->embed(d, values[i]);
        }
    }
}

void Terrain::constructOrdering() const {
    auto rg = RandomGenerator(0, point_space_limit);
    std::vector<unsigned> values(point_space_limit+1);
    std::iota(values.begin(), values.end(), 0);
    std::ranges::shuffle(values, rg.getRNG());
    for (unsigned i = 0; i < vertices.size(); i++) {
        const auto ev = std::dynamic_pointer_cast<EmbeddedVertex>(vertices[i]);
        const auto size = static_cast<unsigned>(static_cast<double>(point_space_limit) / static_cast<double>(vertices.size()));
        ev->embed(0, size * i + size/2);
        ev->embed(1, values[i]);
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

void Terrain::constructE(bool complete) {
    edges.clear();
    for (unsigned i = 0; i < vertices.size() - 1; i++) {
        edges.emplace_back(vertices[i], vertices[i + 1]);
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

double TerrainVisibilityGraph::orientation(
    const double px, const double py,
    const double qx, const double qy,
    const double rx, const double ry
) {
    // | px  py  1 |
    // | qx  qy  1 |
    // | rx  ry  1 |
    return px * qy + py * rx + qx * ry - rx * qy - ry * px - qx * py;
}