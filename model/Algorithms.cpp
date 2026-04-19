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

double BicliquePartitioner::adjustHyperplane(std::vector<EmbeddedVertex>& ordering, double hyperplane) {
    double minDistance = std::numeric_limits<double>::infinity();
    double next = hyperplane;

    for (const auto& vertex : ordering) {
        if (const double dist = vertex.dimensions[1] - hyperplane; dist < minDistance and dist > 0) {
            minDistance = dist;
            next = vertex.dimensions[1];
        }
    }
    return (hyperplane + next) / 2;
}


double BicliquePartitioner::partition(const std::shared_ptr<ComparabilityGraph>& G, bool render) {
    double median = findHyperplane(G->ordering, 1);

    std::vector<EmbeddedVertex> V1_prime;
    std::vector<EmbeddedVertex> V2_prime;

    std::ranges::copy_if(G->ordering, std::back_inserter(V1_prime),
                         [median, &G](const EmbeddedVertex& vertex) {
                             return G->isInV1(vertex.v) && vertex.dimensions[1] <= median;
                         });
    std::ranges::copy_if(G->ordering, std::back_inserter(V2_prime),
                         [median, &G](const EmbeddedVertex& vertex) {
                             return G->isInV2(vertex.v) && vertex.dimensions[1] > median;
                         });

    if (render) median = adjustHyperplane(G->ordering, median);
    return median;
}
