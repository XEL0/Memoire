#ifndef ANALYSIS_GRAPH_HPP
#define ANALYSIS_GRAPH_HPP
#include <unordered_set>
#include <algorithm>
#include <ostream>
#include <ranges>
#include <utility>
#include <vector>
#include <iostream>

#include "RandomGenerator.hpp"
#include "vertex.hpp"


constexpr const char* BLUE = "\033[34m";
constexpr const char* RED = "\033[31m";
constexpr const char* RESET = "\033[0m";

class MultiEdges {
protected:
    std::unordered_set<Vertex*> V1;
    std::unordered_set<Vertex*> V2;
public:
    MultiEdges(std::unordered_set<Vertex*> V1, std::unordered_set<Vertex*> V2);
    ~MultiEdges() = default;
    [[nodiscard]] std::unordered_set<Vertex*> getV1() const { return V1; }
    [[nodiscard]] std::unordered_set<Vertex*> getV2() const { return V2; }
    [[nodiscard]] bool contains(Vertex* v) const { return v->isInV1() ? V1.contains(v) : V2.contains(v);}
};



class Graph {
    friend std::ostream& operator<<(std::ostream& os, const Graph& g);
    friend class Algorithms;
public:
    //Graph(): p(0), q(0), dim(0) {}
    explicit Graph(const std::vector<Vertex*>& vertices, const unsigned p, const unsigned q, const unsigned dim):
        vertices(vertices), p(p), q(q), dim(dim) {}


    [[nodiscard]] auto enumerate() const { return std::views::all(vertices); }
    [[nodiscard]] auto reverseEnumerate() const { return std::views::reverse(vertices); }
    [[nodiscard]] unsigned size() const { return vertices.size(); }
    [[nodiscard]] unsigned V1_size() const { return p; }
    [[nodiscard]] unsigned V2_size() const { return q; }
    [[nodiscard]] unsigned getDimension() const { return dim; }

protected:
    std::vector<Vertex*> vertices;
    unsigned p;
    unsigned q;
    unsigned dim;
};

inline std::ostream& operator<<(std::ostream& os, const Graph& g) {
    os << "V: ";

    for (const auto& v : g.enumerate()) {
        os << (v->isInV1() ? BLUE : RED) << v->getId() << RESET << " (";

        for (unsigned d = 0; d < g.getDimension(); d++) {
            os << v->at(d) << ", ";
        }
        os << "), ";
    }
    return os;
}

class GraphGenerator {
protected:
    std::vector<Vertex*> vertices;
    unsigned point_space_limit = -1;

    void constructV(const unsigned p, const unsigned q, const unsigned dim) {
        const unsigned size = p + q;
        vertices.reserve(size);
        auto rg = RandomGenerator(0, size);
        std::vector<unsigned> ids(size);
        std::iota(ids.begin(), ids.end(), 0);
        std::ranges::shuffle(ids, rg.getRNG());
        int i = 0;
        for (const auto id : ids) {
            const int color = (i < p) ? 0 : 1;
            vertices.push_back(new Vertex(id, color, std::vector<unsigned>(dim)));
            i++;
        }
    }

    void constructOrdering(const unsigned size, const unsigned dim) const {
        auto rg = RandomGenerator(0, size);
        std::vector<unsigned> values(size+1);
        std::iota(values.begin(), values.end(), 0);

        for (unsigned d = 0; d < dim; d++) {
            std::ranges::shuffle(values, rg.getRNG());
            for (unsigned i = 0; i < vertices.size(); i++) {
                vertices[i]->embed(d, values[i]);
            }
        }
    }
public:
    const std::vector<Vertex*>& generate(const unsigned p, const unsigned q, const unsigned dim) {
        point_space_limit = std::max(1000u, (p+q) * 2);
        constructV(p, q, dim);
        constructOrdering(p+q, dim);
        return vertices;
    }

    const std::vector<Vertex*>& generatePreset1() {
        vertices.push_back( new Vertex(3, 0, std::vector<unsigned>{146, 394}));
        vertices.push_back( new Vertex(1, 0, std::vector<unsigned>{457, 841}));
        vertices.push_back( new Vertex(6, 0, std::vector<unsigned>{379, 17}));
        vertices.push_back( new Vertex(5, 0, std::vector<unsigned>{95, 467}));
        vertices.push_back( new Vertex(2, 0, std::vector<unsigned>{197, 530}));
        vertices.push_back( new Vertex(7, 1, std::vector<unsigned>{3, 756}));
        vertices.push_back( new Vertex(8, 1, std::vector<unsigned>{426, 705}));
        vertices.push_back( new Vertex(0, 1, std::vector<unsigned>{896, 316}));
        vertices.push_back( new Vertex(4, 1, std::vector<unsigned>{196, 336}));
        vertices.push_back( new Vertex(9, 1, std::vector<unsigned>{831, 118}));
        return vertices;
    };

    const std::vector<Vertex*>& generatePreset2() {
        vertices.push_back(new Vertex(1, 0, std::vector<unsigned>{100, 200}));
        vertices.push_back(new Vertex(2, 0, std::vector<unsigned>{200, 400}));
        vertices.push_back(new Vertex(3, 0, std::vector<unsigned>{300, 300}));
        vertices.push_back(new Vertex(0, 1, std::vector<unsigned>{0, 0}));
        vertices.push_back(new Vertex(4, 1, std::vector<unsigned>{400, 500}));
        vertices.push_back(new Vertex(5, 1, std::vector<unsigned>{500, 100}));
        vertices.push_back(new Vertex(6, 1, std::vector<unsigned>{600, 600}));
        vertices.push_back(new Vertex(7, 1, std::vector<unsigned>{700, 700}));
        return vertices;
    }


    GraphGenerator() = default;
    ~GraphGenerator() {
        for (const auto v : vertices) {
            delete v;
        }
    }
    GraphGenerator(const GraphGenerator&) = delete;
    GraphGenerator& operator=(const GraphGenerator&) = delete;

};

#endif //ANALYSIS_GRAPH_HPP
