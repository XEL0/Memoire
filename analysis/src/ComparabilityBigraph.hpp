#ifndef ANALYSIS_GRAPH_HPP
#define ANALYSIS_GRAPH_HPP
#include <unordered_set>
#include <algorithm>
#include <ostream>
#include <ranges>
#include <utility>
#include <vector>
#include <iostream>

#include "Bigraph.hpp"
#include "RandomGenerator.hpp"


class Vertex {
protected:
    unsigned id;
    unsigned color;
    std::vector<unsigned> embedding;

    [[nodiscard]] bool isSmallerTo(const Vertex& v) const {
        return std::ranges::all_of(
                std::views::zip(embedding, v.embedding),
                [](auto pair) { auto [a, b] = pair; return a < b; });
    }
    [[nodiscard]] bool isGreaterTo(const Vertex& v) const {
        return std::ranges::all_of(
                        std::views::zip(embedding, v.embedding),
                        [](auto pair) { auto [a, b] = pair; return a > b; });
    }

public:
    Vertex(const unsigned id, const unsigned color, const std::vector<unsigned>& embedding) : id(id), color(color), embedding(embedding) {}
    virtual ~Vertex() = default;

    [[nodiscard]] unsigned getId() const { return id; }
    [[nodiscard]] const std::vector<unsigned>& getEmbedding() const { return embedding; }
    [[nodiscard]] unsigned at(const unsigned dim) const { return embedding[dim];}
    [[nodiscard]] bool isComparableTo(const Vertex &v) const { return isInV1() ? isSmallerTo(v) : isGreaterTo(v); }
    [[nodiscard]] bool isInV1() const { return color == 0;}
    [[nodiscard]] bool isInV2() const { return color == 1;}
    [[nodiscard]] bool getDim() const { return embedding.size(); }

    void embed(const unsigned dim, const unsigned value) { embedding[dim] = value;}

    bool operator==(const Vertex& other) const { return id == other.id; }
};


template<>
struct std::hash<Vertex> {
    size_t operator()(const Vertex& v) const {
        size_t h = std::hash<unsigned>{}(v.getId());
        h ^= std::hash<unsigned>{}(v.isInV1()) + 0x9e3779b9 + (h << 6) + (h >> 2);
        for (const unsigned x : v.getEmbedding()) { h ^= std::hash<unsigned>{}(x) + 0x9e3779b9 + (h << 6) + (h >> 2);}
        return h;
    }
};




class ComparabilityBigraph {
    friend std::ostream& operator<<(std::ostream& os, const ComparabilityBigraph& g);
    friend class Algorithms;
public:
    //Graph(): p(0), q(0), dim(0) {}
    explicit ComparabilityBigraph(const std::vector<Vertex*>& vertices, const unsigned p, const unsigned q, const unsigned dim):
        vertices(vertices), p(p), q(q), dim(dim) {}


    [[nodiscard]] auto enumerate() const { return std::views::all(vertices); }
    [[nodiscard]] auto reverseEnumerate() const { return std::views::reverse(vertices); }
    [[nodiscard]] unsigned size() const { return vertices.size(); }
    [[nodiscard]] unsigned V1_size() const { return p; }
    [[nodiscard]] unsigned V2_size() const { return q; }
    [[nodiscard]] unsigned getDimension() const { return dim; }
    
    [[nodiscard]] Biclique toBiclique() const {
        std::unordered_set<unsigned> V1, V2;
        for (const auto& v : vertices) {
            v->isInV1() ? V1.insert(v->getId()) : V2.insert(v->getId());
        }
        return {V1, V2};
    }
    bool comparable(Vertex* &u, Vertex* &v) {
        return u->isComparableTo(*v);
    }
    bool diffColor(Vertex* &u, Vertex* &v) {
        return u->isInV1() != v->isInV1();
    }

    [[nodiscard]] bool isComplete() {
        for (auto it = vertices.begin(); it != vertices.end(); ++it) {
            for (auto jt = std::next(it); jt != vertices.end(); ++jt) {
                if (diffColor(*it, *jt) and not comparable(*it, *jt)) return false;
            }
        }
        return true;
    }

protected:
    std::vector<Vertex*> vertices;
    unsigned p;
    unsigned q;
    unsigned dim;
};

inline std::ostream& operator<<(std::ostream& os, const ComparabilityBigraph& g) {
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


    const std::vector<Vertex*>& generatePreset3() {
	vertices.push_back(new Vertex(36, 0, std::vector<unsigned>{20, 31}));
	vertices.push_back(new Vertex(0, 0, std::vector<unsigned>{5, 5}));
	vertices.push_back(new Vertex(34, 0, std::vector<unsigned>{0, 23}));
	vertices.push_back(new Vertex(33, 0, std::vector<unsigned>{8, 29}));
	vertices.push_back(new Vertex(35, 0, std::vector<unsigned>{33, 21}));
	vertices.push_back(new Vertex(37, 0, std::vector<unsigned>{22, 25}));
	vertices.push_back(new Vertex(38, 0, std::vector<unsigned>{27, 12}));
	vertices.push_back(new Vertex(10, 0, std::vector<unsigned>{25, 14}));
	vertices.push_back(new Vertex(1, 0, std::vector<unsigned>{18, 36}));
	vertices.push_back(new Vertex(31, 0, std::vector<unsigned>{31, 4}));
	vertices.push_back(new Vertex(29, 0, std::vector<unsigned>{39, 28}));
	vertices.push_back(new Vertex(6, 0, std::vector<unsigned>{15, 19}));
	vertices.push_back(new Vertex(24, 0, std::vector<unsigned>{36, 1}));
	vertices.push_back(new Vertex(25, 0, std::vector<unsigned>{9, 24}));
	vertices.push_back(new Vertex(19, 0, std::vector<unsigned>{28, 3}));
	vertices.push_back(new Vertex(13, 0, std::vector<unsigned>{40, 22}));
	vertices.push_back(new Vertex(20, 0, std::vector<unsigned>{17, 26}));
	vertices.push_back(new Vertex(28, 0, std::vector<unsigned>{24, 0}));
	vertices.push_back(new Vertex(17, 0, std::vector<unsigned>{21, 32}));
	vertices.push_back(new Vertex(12, 0, std::vector<unsigned>{30, 39}));
	vertices.push_back(new Vertex(14, 1, std::vector<unsigned>{2, 38}));
	vertices.push_back(new Vertex(8, 1, std::vector<unsigned>{32, 15}));
	vertices.push_back(new Vertex(15, 1, std::vector<unsigned>{26, 40}));
	vertices.push_back(new Vertex(26, 1, std::vector<unsigned>{1, 35}));
	vertices.push_back(new Vertex(39, 1, std::vector<unsigned>{37, 37}));
	vertices.push_back(new Vertex(9, 1, std::vector<unsigned>{10, 16}));
	vertices.push_back(new Vertex(7, 1, std::vector<unsigned>{34, 11}));
	vertices.push_back(new Vertex(11, 1, std::vector<unsigned>{35, 13}));
	vertices.push_back(new Vertex(3, 1, std::vector<unsigned>{6, 2}));
	vertices.push_back(new Vertex(16, 1, std::vector<unsigned>{13, 27}));
	vertices.push_back(new Vertex(30, 1, std::vector<unsigned>{16, 30}));
	vertices.push_back(new Vertex(27, 1, std::vector<unsigned>{38, 6}));
	vertices.push_back(new Vertex(18, 1, std::vector<unsigned>{11, 8}));
	vertices.push_back(new Vertex(23, 1, std::vector<unsigned>{3, 18}));
	vertices.push_back(new Vertex(5, 1, std::vector<unsigned>{19, 17}));
	vertices.push_back(new Vertex(32, 1, std::vector<unsigned>{23, 33}));
	vertices.push_back(new Vertex(2, 1, std::vector<unsigned>{4, 9}));
	vertices.push_back(new Vertex(22, 1, std::vector<unsigned>{29, 10}));
	vertices.push_back(new Vertex(21, 1, std::vector<unsigned>{14, 20}));
	vertices.push_back(new Vertex(4, 1, std::vector<unsigned>{7, 7}));
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
