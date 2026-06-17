#ifndef TOOL_BIGRAPH_HPP
#define TOOL_BIGRAPH_HPP

#include <algorithm>
#include <ranges>
#include <unordered_set>
#include <utility>
#include <vector>


class Biclique {
    friend std::ostream& operator<<(std::ostream& os, const Biclique& g);
protected:
    std::unordered_set<unsigned> V1;
    std::unordered_set<unsigned> V2;
public:
    Biclique(std::unordered_set<unsigned> V1, std::unordered_set<unsigned> V2): V1(std::move(V1)), V2(std::move(V2)) {}
    ~Biclique() = default;
    [[nodiscard]] unsigned p() const { return V1.size(); }
    [[nodiscard]] unsigned q() const { return V2.size(); }
    [[nodiscard]] std::unordered_set<unsigned> getV1() const { return V1; }
    [[nodiscard]] std::unordered_set<unsigned> getV2() const { return V2; }
    [[nodiscard]] bool contains(const unsigned v) const { return V1.contains(v) or V2.contains(v);}
    [[nodiscard]] bool containsP(const unsigned v) const { return V1.contains(v); }
    [[nodiscard]] bool containsQ(const unsigned v) const { return V2.contains(v); }
};

inline std::ostream& operator<<(std::ostream& os, const Biclique& g) {
    for (const auto& v : g.V1) {
        os << "\033[34m" << v << "\033[0m" << " ";
    }
    for (const auto& v : g.V2) {
        os << "\033[31m" << v << "\033[0m" << " ";
    }
    return os;
}

class GraphOfBicliques {
    friend class Algorithms;
protected:
    std::vector<Biclique> bicliques;
    unsigned p;
    unsigned q;
public:
    GraphOfBicliques(const std::vector<Biclique>& bicliques, const unsigned p, const unsigned q): bicliques(bicliques), p(p), q(q) {}
    ~GraphOfBicliques() = default;
    [[nodiscard]] int getColor(const unsigned v) const {
        for (const auto& b : bicliques) {
            if (b.contains(v)) return b.containsP(v) ? 0 : 1;
        }
        return -1;
    }
};

#endif //TOOL_BIGRAPH_HPP
