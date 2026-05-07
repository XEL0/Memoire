#ifndef ANALYSIS_VERTEX_HPP
#define ANALYSIS_VERTEX_HPP

#include <algorithm>
#include <ranges>
#include <vector>

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
    [[nodiscard]] bool isComparableTo(const Vertex &v) const { return isSmallerTo(v) or isGreaterTo(v); }
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

#endif //ANALYSIS_VERTEX_HPP
