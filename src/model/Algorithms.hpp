#ifndef MEMOIRE_ALGORITHMS_HPP
#define MEMOIRE_ALGORITHMS_HPP
#include "Graph.hpp"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Algorithms {
};

class BicliquePartitioner : public Algorithms {
protected:
    [[nodiscard]] double findHyperplane(const std::shared_ptr<ComparabilityBigraph>& G, unsigned dim) const;
    [[nodiscard]] double adjustHyperplane(const std::shared_ptr<ComparabilityBigraph>& G, double hyperplane) const;
    [[nodiscard]] bool areAllV1LessThanV2(const std::shared_ptr<ComparabilityBigraph> &G) const;

    [[nodiscard]] std::vector<std::shared_ptr<ComparabilityBigraph>> partition_light(
    const std::shared_ptr<ComparabilityBigraph>& G, bool optimize_size) const;

    [[nodiscard]] std::vector<std::shared_ptr<ComparabilityBigraph>> partition_save(
        const std::shared_ptr<ComparabilityBigraph>& G, bool optimize_size, json& node) const;
public:
    explicit BicliquePartitioner();


    [[nodiscard]] std::vector<std::shared_ptr<ComparabilityBigraph>> partition(
        const std::shared_ptr<ComparabilityBigraph>& G,
        bool optimize_size,
        bool render=true) const;

};


#endif //MEMOIRE_ALGORITHMS_HPP