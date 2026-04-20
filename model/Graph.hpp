#ifndef MEMOIRE_GRAPH_HPP
#define MEMOIRE_GRAPH_HPP

#include <array>
#include <memory>
#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using Vertex = unsigned;

struct EmbeddedVertex {
    Vertex v;
    std::vector<unsigned> dimensions;
};

using Edge = std::pair<Vertex, Vertex>;

class Graph {
protected:
    std::unique_ptr<std::unordered_set<Vertex>> V1;
    std::unique_ptr<std::vector<Edge>> E;

    class VertexRange {
    public:
        class iterator {
        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = Vertex;
            using difference_type = std::ptrdiff_t;

            iterator(const std::array<std::unordered_set<Vertex>*, 2>& sets, std::size_t setCount, std::size_t setIndex)
                : sets(sets), setCount(setCount), setIndex(setIndex) {
                advanceToValidSet();
            }

            Vertex operator*() const {
                return *it;
            }

            iterator& operator++() {
                ++it;
                if (setIndex < setCount && it == sets[setIndex]->cend()) {
                    ++setIndex;
                    advanceToValidSet();
                }
                return *this;
            }

            bool operator==(const iterator& other) const {
                if (setIndex != other.setIndex) {
                    return false;
                }
                if (setIndex == setCount) {
                    return true;
                }
                return it == other.it;
            }

        private:
            void advanceToValidSet() {
                while (setIndex < setCount) {
                    const auto* set = sets[setIndex];
                    if (set != nullptr && !set->empty()) {
                        it = set->cbegin();
                        return;
                    }
                    ++setIndex;
                }
            }

            const std::array<std::unordered_set<Vertex>*, 2>& sets;
            std::size_t setCount;
            std::size_t setIndex;
            std::unordered_set<Vertex>::const_iterator it{};
        };

        VertexRange(std::unordered_set<Vertex>* a)
            : sets{ a, nullptr }, setCount(1) {}

        VertexRange(std::unordered_set<Vertex>* a, std::unordered_set<Vertex>* b)
            : sets{ a, b }, setCount(2) {}

        iterator begin() const {
            return iterator(sets, setCount, 0);
        }

        iterator end() const {
            return iterator(sets, setCount, setCount);
        }

    private:
        std::array<std::unordered_set<Vertex>*, 2> sets{};
        std::size_t setCount{};
    };

    bool bipartite = false;
    bool embedded = false;
    virtual std::unique_ptr<std::vector<Edge>> constructAllPossibleEdges();
    virtual void constructEdgesSet();
    virtual void constructV(unsigned p, unsigned q);
    [[nodiscard]] virtual bool comparable(Vertex u, Vertex v) const;
    Graph();
public:
    Graph(std::unique_ptr<std::unordered_set<Vertex>> V, std::unique_ptr<std::vector<Edge>> E);
    Graph(unsigned V);
    virtual ~Graph() = default;

    [[nodiscard]] virtual VertexRange vertices() const {
        return VertexRange(V1.get());
    }

    [[nodiscard]] auto edges() const {
        return std::views::all(*E);
    }

    [[nodiscard]] virtual bool isEmbedded() const { return embedded; }
    [[nodiscard]] virtual bool isBipartite() const { return bipartite; }
};

class BipartiteGraph : virtual public Graph {
protected:
    std::unique_ptr<std::unordered_set<Vertex>> V2;

    void constructV(unsigned p, unsigned q) override;
    [[nodiscard]] bool comparable(Vertex u, Vertex v) const override;
    BipartiteGraph();
public:
    BipartiteGraph(std::unique_ptr<std::unordered_set<Vertex>> V1,
                   std::unique_ptr<std::unordered_set<Vertex>> V2,
                   std::unique_ptr<std::vector<Edge>> E);
    BipartiteGraph(unsigned p, unsigned q);
    ~BipartiteGraph() override = default;

    [[nodiscard]] bool isInV1(const Vertex v) const {
        return V1->contains(v);
    }

    [[nodiscard]] bool isInV2(const Vertex v) const {
        return V2->contains(v);
    }

    [[nodiscard]] VertexRange vertices() const override {
        return VertexRange(V1.get(), V2.get());
    }
};

class ComparabilityGraph : virtual public Graph {
    friend class BicliquePartitioner;
protected:
    unsigned dim;
    unsigned point_space_limit;
    std::unique_ptr<std::unordered_map<Vertex, std::vector<unsigned>>> FA_ordering;
    std::unique_ptr<std::vector<EmbeddedVertex>> ordering;

    [[nodiscard]] bool comparable(Vertex u, Vertex v) const override;

    void constructOrdering();
    ComparabilityGraph();
public:
    ComparabilityGraph(std::unique_ptr<std::unordered_set<Vertex>> V,
                       std::unique_ptr<std::vector<Edge>> E,
                       std::unique_ptr<std::unordered_map<Vertex, std::vector<unsigned>>> ordering,
                       unsigned dim,
                       unsigned point_space_limit);
    ComparabilityGraph(unsigned V, unsigned dim, unsigned point_space_limit);
    ~ComparabilityGraph() override = default;

    [[nodiscard]] auto embedding() const {
        return std::views::all(*FA_ordering);
    }

    [[nodiscard]] virtual unsigned getPointSpaceLimit() const { return point_space_limit; }
};

class ComparabilityBigraph : virtual public BipartiteGraph, virtual public ComparabilityGraph {
protected:
    [[nodiscard]] bool comparable(Vertex u, Vertex v) const override;
    ComparabilityBigraph();

public:
    ComparabilityBigraph(std::unique_ptr<std::unordered_set<Vertex>> V1,
                         std::unique_ptr<std::unordered_set<Vertex>> V2,
                         std::unique_ptr<std::vector<Edge>> E,
                         std::unique_ptr<std::unordered_map<Vertex, std::vector<unsigned>>> ordering,
                         unsigned dim,
                         unsigned point_space_limit);

    ComparabilityBigraph(unsigned p, unsigned q, unsigned dim, unsigned point_space_limit);
    ~ComparabilityBigraph() override = default;
};

#endif //MEMOIRE_GRAPH_HPP