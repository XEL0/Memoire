#include "Algorithms.hpp"

#include <algorithm>
#include <iostream>

BicliquePartitioner::BicliquePartitioner() = default;


double BicliquePartitioner::findHyperplane(const std::shared_ptr<ComparabilityBigraph>& G, unsigned dim) const {
    const std::size_t n = (G->size() - 1) / 2;
    std::ranges::nth_element(G->vertices, G->vertices.begin() + static_cast<std::ptrdiff_t>(n),
                             [dim, G](const VertexPointer& a, const VertexPointer& b) {
                                 return G->getEmbeddingAt(a, dim) < G->getEmbeddingAt(b, dim);
                             });
    const auto at = G->vertices.begin() + + static_cast<std::ptrdiff_t>(n);
    return G->getEmbeddingAt(*at, dim);
}

double BicliquePartitioner::adjustHyperplane(const std::shared_ptr<ComparabilityBigraph>& G, const double hyperplane) const {
    double minDistance = std::numeric_limits<double>::infinity();
    double next = hyperplane;

    for (const auto& v : G->vertices) {
        const auto pos = G->getEmbeddingAt(v, G->dim - 1);
        if (const double dist = pos - hyperplane; dist < minDistance and dist > 0) {
            minDistance = dist;
            next = pos;
        }
    }
    return (hyperplane + next) / 2;
}


std::vector<std::shared_ptr<ComparabilityBigraph>> BicliquePartitioner::partition(
    const std::shared_ptr<ComparabilityBigraph>& G,
    const bool optimize_size,
    const bool render) const {

    if (G->size() <= 1) {
        return {};
    }

    std::cout << *G << std::endl;
    if (optimize_size) {
        if (areAllV1LessThanV2(G)) {
            std::cout << "optimized" << std::endl;
            return {G};
        }
    }

    if (G->dim == 0) {
        return std::vector{G};
    }

    double H = findHyperplane(G, G->dim-1);

    auto V_under_H = std::vector<VertexPointer>{};
    auto V_over_H = std::vector<VertexPointer>{};
    auto V_prime = std::vector<VertexPointer>{};

    unsigned nb_v1_under = 0;
    unsigned nb_v2_under = 0;
    unsigned nb_v1_over = 0;
    unsigned nb_v2_over = 0;

    for (const auto& v : G->enumerate()) {
        if (G->getEmbeddingAt(v, G->dim-1) <= H) {
            V_under_H.push_back(v);
            if (G->isInV1(v)) {
                nb_v1_under++;
                V_prime.push_back(v);
            } else {
                nb_v2_under++;
            }

        } else {
            V_over_H.push_back(v);
            if (G->isInV2(v)) {
                nb_v2_over++;
                V_prime.push_back(v);
            } else {
                nb_v1_over++;
            }
        }
    }

    if (render) H = adjustHyperplane(G, H);
    std::cout << "H = " << H << std::endl;
    auto res = std::vector<std::shared_ptr<ComparabilityBigraph>>{};

    if (nb_v1_under and nb_v2_over) {
        const auto flattened_CG = std::make_shared<ComparabilityBigraph>(
    std::move(V_prime), nb_v1_under, nb_v2_over, G->dim-1, G->getPointSpaceLimit());
        res = partition(flattened_CG, optimize_size, render);
    }

    if (nb_v2_under) {
        const auto under_H_CG = std::make_shared<ComparabilityBigraph>(
            std::move(V_under_H), nb_v1_under, nb_v2_under, G->dim, G->getPointSpaceLimit());
        const auto part2 = partition(under_H_CG, optimize_size, render);
        res.insert(res.end(), part2.begin(), part2.end());
    }

    if (nb_v1_over) {
        const auto over_H_CG = std::make_shared<ComparabilityBigraph>(
            std::move(V_over_H), nb_v1_over, nb_v2_over, G->dim, G->getPointSpaceLimit());
        const auto part3 = partition(over_H_CG, optimize_size, render);
        res.insert(res.end(), part3.begin(), part3.end());
    }
    return res;
}

bool BicliquePartitioner::areAllV1LessThanV2(const std::shared_ptr<ComparabilityBigraph>& G) const {
    return G->isComplete();
}
