#include "Algorithms.hpp"

#include <algorithm>
#include <iostream>

BicliquePartitioner::BicliquePartitioner() = default;

double BicliquePartitioner::findHyperplane(std::vector<EmbeddedVertex>& ordering, unsigned dim) {
    const std::size_t n = (ordering.size() - 1) / 2;

    for (auto& [v, pos] : ordering) {
        std::cout << v << " (" << pos[0] << ", " << pos[1] << ")" << std::endl;
    }
    std::ranges::nth_element(ordering, ordering.begin() + n,
                             [dim](const EmbeddedVertex& a, const EmbeddedVertex& b) {
                                 return a.dimensions[dim] < b.dimensions[dim];
                             });
    for (auto& [v, pos] : ordering) {
        std::cout << v << " (" << pos[0] << ", " << pos[1] << ")" << std::endl;
    }

    return ordering[n].dimensions[dim];
}

double BicliquePartitioner::partition(const std::shared_ptr<ComparabilityGraph>& G) {
    double median = findHyperplane(G->ordering, 1);
    return median;
}
