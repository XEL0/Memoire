#include "Algorithms.hpp"

#include <algorithm>
#include <iostream>

BicliquePartitioner::BicliquePartitioner() = default;



auto to_vector(std::unique_ptr<std::unordered_map<Vertex, std::vector<unsigned>>>& map){
    std::vector<std::vector<unsigned>*> items;
    items.reserve(map->size());

    for (auto &[v, pos] : *map) {
        items.push_back(&pos);
    }
    return items;
}

double BicliquePartitioner::findHyperplane(std::unique_ptr<std::unordered_map<Vertex, std::vector<unsigned>>>& ordering, unsigned dim) {
    const std::size_t n = (ordering->size() - 1) / 2;
    auto ordering_vec = to_vector(ordering);

    std::ranges::nth_element(ordering_vec, ordering_vec.begin() + static_cast<std::ptrdiff_t>(n),
                             [dim](const std::vector<unsigned>* a, const std::vector<unsigned>* b) {
                                 return (*a)[dim] < (*b)[dim];
                             });

    return (*ordering_vec[n])[dim];
}

double BicliquePartitioner::adjustHyperplane(std::unique_ptr<std::unordered_map<Vertex, std::vector<unsigned>>>& ordering, double hyperplane) {
    double minDistance = std::numeric_limits<double>::infinity();
    double next = hyperplane;

    for (const auto& [v, pos] : *ordering) {
        if (const double dist = pos[1] - hyperplane; dist < minDistance and dist > 0) {
            minDistance = dist;
            next = pos[1];
        }
    }
    return (hyperplane + next) / 2;
}


double BicliquePartitioner::partition(const std::shared_ptr<ComparabilityBigraph>& G, const bool render) {
    double median = findHyperplane(G->ordering, 1);

    std::vector<EmbeddedVertex> V1_prime;
    std::vector<EmbeddedVertex> V2_prime;

    /*std::ranges::copy_if(*G->ordering, std::back_inserter(V1_prime),
                         [median, &G](const EmbeddedVertex& vertex) {
                             return G->isInV1(vertex.v) && vertex.dimensions[1] <= median;
                         });
    std::ranges::copy_if(*G->ordering, std::back_inserter(V2_prime),
                         [median, &G](const EmbeddedVertex& vertex) {
                             return G->isInV2(vertex.v) && vertex.dimensions[1] > median;
                         });*/

    if (render) median = adjustHyperplane(G->ordering, median);
    return median;
}
