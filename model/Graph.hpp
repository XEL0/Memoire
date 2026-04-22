#ifndef MEMOIRE_GRAPH_HPP
#define MEMOIRE_GRAPH_HPP

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <ostream>
#include <ranges>
#include <utility>
#include <vector>

constexpr const char* BLUE = "\033[34m";
constexpr const char* RED = "\033[31m";
constexpr const char* RESET = "\033[0m";

class Vertex {
protected:
    unsigned id;
public:
    explicit Vertex(const unsigned id) : id(id) {}
    virtual ~Vertex() = default;
    [[nodiscard]] unsigned getId() const { return id; }
    [[nodiscard]] bool isComparableTo(const Vertex& v) const { return true; }

};

class ColoredVertex : public virtual Vertex {
protected:
    unsigned color;
public:
    ColoredVertex(const unsigned id, const unsigned color) : Vertex(id), color(color) {}
    ~ColoredVertex() override = default;
    [[nodiscard]] bool isInV1() const { return color == 0;}
    [[nodiscard]] bool isInV2() const { return color == 1;}
    [[nodiscard]] bool isComparableTo(const ColoredVertex &v) const{ return this->isInV1() xor v.isInV1(); }
};

class EmbeddedVertex : public virtual Vertex {
protected:
    std::vector<unsigned> embedding;
    [[nodiscard]] bool isSmallerTo(const EmbeddedVertex& v) const {
        return std::ranges::all_of(
                std::views::zip(embedding, v.embedding),
                [](auto pair) { auto [a, b] = pair; return a < b; });
    }

    [[nodiscard]] bool isGreaterTo(const EmbeddedVertex& v) const {
        return std::ranges::all_of(
                        std::views::zip(embedding, v.embedding),
                        [](auto pair) { auto [a, b] = pair; return a > b; });
    }
public:
    EmbeddedVertex(const unsigned id, const std::vector<unsigned>& embedding) : Vertex(id), embedding(embedding) {}
    ~EmbeddedVertex() override = default;
    [[nodiscard]] const std::vector<unsigned>& getEmbedding() const { return embedding; }
    [[nodiscard]] unsigned at(const unsigned dim) const {
        assert(dim < embedding.size());
        return embedding[dim];
    }
    [[nodiscard]] bool isComparableTo(const EmbeddedVertex &v) const { return isSmallerTo(v) or isGreaterTo(v); }
    void embed(const unsigned dim, const unsigned value) { embedding[dim] = value;}

};

class ColoredEmbeddedVertex : public virtual ColoredVertex, public virtual EmbeddedVertex {
public:
    ColoredEmbeddedVertex(const unsigned id, const unsigned color, const std::vector<unsigned>& embedding) : ColoredVertex(id, color), Vertex(id), EmbeddedVertex(id, embedding) {}
    [[nodiscard]] bool isComparableTo(const ColoredEmbeddedVertex &v) const {
        if (not ColoredVertex::isComparableTo(v)) return false;
        return isInV1() ? isSmallerTo(v) : isGreaterTo(v);
    }
};

using VertexPointer = std::shared_ptr<Vertex>;
using Edge = std::pair<VertexPointer, VertexPointer>;

class Graph {
    friend std::ostream& operator<<(std::ostream& os, const Graph& g);
protected:
    std::vector<VertexPointer> vertices;
    [[nodiscard]] virtual bool comparable(const VertexPointer &u, const VertexPointer &v) const { return true; }
    virtual void constructV(unsigned size);

public:
    Graph();
    explicit Graph(std::vector<VertexPointer> vertices);
    virtual ~Graph() = default;

    [[nodiscard]] auto enumerate() const { return std::views::all(vertices); }
    [[nodiscard]] auto reverseEnumerate() const { return std::views::reverse(vertices); }
    [[nodiscard]] unsigned size() const { return vertices.size(); }

    void generate(unsigned size);
    std::unique_ptr<std::vector<Edge>> makeComplete();
};

class Bigraph : virtual public Graph {
    friend std::ostream& operator<<(std::ostream& os, const Bigraph& g);
protected:
    [[nodiscard]] bool comparable(const VertexPointer &u, const VertexPointer &v) const override;
    virtual void constructV(unsigned p, unsigned q);
    unsigned p{};
    unsigned q{};
public:
    Bigraph();
    explicit Bigraph(std::vector<VertexPointer> vertices, unsigned p, unsigned q);
    ~Bigraph() override = default;
    void generate(unsigned p, unsigned q);
    [[nodiscard]] bool isInV1(const VertexPointer& v) const { return dynamic_cast<ColoredVertex*>(v.get())->isInV1(); }
    [[nodiscard]] bool isInV2(const VertexPointer& v) const { return dynamic_cast<ColoredVertex*>(v.get())->isInV2(); }
};

class ComparabilityGraph : virtual public Graph {
    friend std::ostream& operator<<(std::ostream& os, const ComparabilityGraph& g);
    friend class BicliquePartitioner;
protected:
    unsigned dim;
    unsigned point_space_limit;
    [[nodiscard]] bool comparable(const VertexPointer &u, const VertexPointer &v) const override;

    void constructV(unsigned size) override;

    virtual void constructOrdering();
public:
    ComparabilityGraph();
    explicit ComparabilityGraph(std::vector<VertexPointer> vertices, unsigned dim, unsigned point_space_limit);
    ~ComparabilityGraph() override = default;
    void generate(unsigned size, unsigned dim, unsigned point_space_limit);
    [[nodiscard]] virtual unsigned getDimension() const { return dim; }
    [[nodiscard]] virtual unsigned getEmbeddingAt(const VertexPointer& v, const unsigned d) const {
    return dynamic_cast<EmbeddedVertex*>(v.get())->at(d);
}
    [[nodiscard]] virtual unsigned getPointSpaceLimit() const { return point_space_limit; }
};

class ComparabilityBigraph : virtual public Bigraph, virtual public ComparabilityGraph {
    friend std::ostream& operator<<(std::ostream& os, const ComparabilityBigraph& g);
    friend class BicliquePartitioner;
protected:
    [[nodiscard]] bool comparable(const VertexPointer &u, const VertexPointer &v) const override;
    void constructV(unsigned p, unsigned q) override;
public:
    ComparabilityBigraph();
    explicit ComparabilityBigraph(const std::vector<VertexPointer> &vertices, unsigned p, unsigned q, unsigned dim, unsigned point_space_limit);
    ~ComparabilityBigraph() override = default;
    void generate(unsigned p, unsigned q, unsigned dim, unsigned point_space_limit);
    bool isComplete();
};




inline std::ostream &operator<<(std::ostream &os, const Graph &g) {
    os << "V: ";
    for (const auto& v : g.enumerate()) { os << v->getId() << ", "; }
    return os;
}


inline std::ostream& operator<<(std::ostream& os, const Bigraph& g) {
    os << "V: ";
    for (const auto& v : g.enumerate()) {
        os << (g.isInV1(v) ? BLUE : RED) << v->getId() << RESET;
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const ComparabilityGraph& g) {
    os << "V: ";

    for (const auto& v : g.enumerate()) {
        os << v->getId() << " (";

        for (unsigned d = 0; d < g.getDimension(); d++) {
            os << g.getEmbeddingAt(v, d) << ", ";
        }
        os << "), ";
    }
    return os;
}


inline std::ostream& operator<<(std::ostream& os, const ComparabilityBigraph& g) {
    os << "V: ";

    for (const auto& v : g.enumerate()) {
        os << (g.isInV1(v) ? BLUE : RED) << v->getId() << RESET << " (";

        for (unsigned d = 0; d < g.getDimension(); d++) {
            os << g.getEmbeddingAt(v, d) << ", ";
        }
        os << "), ";
    }
    return os;
}


#endif //MEMOIRE_GRAPH_HPP