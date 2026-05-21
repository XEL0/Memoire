#ifndef MEMOIRE_ALGORITHMS_HPP
#define MEMOIRE_ALGORITHMS_HPP
#include "Graph.hpp"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Algorithms {
};

class BicliquePartitioner : public Algorithms {
protected:
    [[nodiscard]] static double findHyperplane(const std::shared_ptr<ComparabilityBigraph>& G, unsigned dim) ;
    [[nodiscard]] static double adjustHyperplane(const std::shared_ptr<ComparabilityBigraph>& G, double hyperplane) ;
    [[nodiscard]]  static bool areAllV1LessThanV2(const std::shared_ptr<ComparabilityBigraph> &G) ;

    [[nodiscard]] static std::vector<std::shared_ptr<ComparabilityBigraph>> partition_save(
        const std::shared_ptr<ComparabilityBigraph>& G, bool optimize_size, json& node);
public:
    explicit BicliquePartitioner();

    [[nodiscard]] static std::vector<std::shared_ptr<ComparabilityBigraph>> partition(
        const std::shared_ptr<ComparabilityBigraph>& G,
        bool optimize_size);

};


class CappedGraphDecomposition : public Algorithms {
protected:
    [[nodiscard]] static std::shared_ptr<ComparabilityBigraph> convertToComparabilityBigraph(
        const std::vector<VertexPointer>& V1,
        const std::vector<VertexPointer>& V2,
        const std::vector<Edge>& edges_H);
    [[nodiscard]] static std::vector<std::shared_ptr<ComparabilityBigraph>> decomposeRecursive(
        const std::shared_ptr<TerrainVisibilityGraph>& G);
    [[nodiscard]] static unsigned findMinNeighbor(const VertexPointer& v, const std::vector<Edge>& edges);
    [[nodiscard]] static unsigned findMaxNeighbor(const VertexPointer& v, const std::vector<Edge>& edges);
    [[nodiscard]] static std::unordered_map<unsigned, std::pair<unsigned, unsigned>> findMinAndMaxNeighborhood(
        const std::vector<VertexPointer>& V1, const std::vector<VertexPointer>& V2, const std::vector<Edge>& edges);

public:
    [[nodiscard]] static std::vector<std::shared_ptr<ComparabilityBigraph>> decompose(
        const std::shared_ptr<TerrainVisibilityGraph>& G);
};

#endif //MEMOIRE_ALGORITHMS_HPP