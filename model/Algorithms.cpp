#include "Algorithms.hpp"

#include <algorithm>
#include <iostream>

BicliquePartitioner::BicliquePartitioner() = default;


double BicliquePartitioner::findHyperplane(const std::shared_ptr<ComparabilityBigraph>& G, unsigned dim) const {
    const std::size_t n = (G->size() - 1) / 2;
    auto vertices = G->vertices();
    std::ranges::nth_element(vertices, vertices.begin() + static_cast<std::ptrdiff_t>(n),
                             [dim, G](const Vertex a, const Vertex b) {
                                 return G->getEmbedding(a)[dim] < G->getEmbedding(b)[dim];
                             });

    return G->getEmbedding(vertices[n])[dim];
}

double BicliquePartitioner::adjustHyperplane(const std::shared_ptr<ComparabilityBigraph>& G, const double hyperplane) const {
    double minDistance = std::numeric_limits<double>::infinity();
    double next = hyperplane;

    for (const auto& v : G->vertices()) {
        auto pos = G->getEmbedding(v);
        if (const double dist = pos[1] - hyperplane; dist < minDistance and dist > 0) {
            minDistance = dist;
            next = pos[1];
        }
    }
    return (hyperplane + next) / 2;
}


std::vector<std::shared_ptr<ComparabilityBigraph>> BicliquePartitioner::partition(const std::shared_ptr<ComparabilityBigraph>& G, const bool render) const {
    const auto dim = G->dim - 1;

    if (G->V1->empty() or G->V2->empty()) {
        return std::vector<std::shared_ptr<ComparabilityBigraph>>{};
    }
    if (dim == 0) {
        return std::vector{G};
    }
    double H = findHyperplane(G, dim);

    auto V1_under_H = std::make_unique<std::unordered_set<Vertex>>();
    auto V2_under_H = std::make_unique<std::unordered_set<Vertex>>();

    auto V1_over_H = std::make_unique<std::unordered_set<Vertex>>();
    auto V2_over_H = std::make_unique<std::unordered_set<Vertex>>();

    auto V1_prime = std::make_unique<std::unordered_set<Vertex>>();
    auto V2_prime = std::make_unique<std::unordered_set<Vertex>>();

    for (auto v : G->enumerateV1()) {
        if (G->getEmbedding(v)[dim] <= H) {
            V1_under_H->insert(v);
            V1_prime->insert(v);
        } else {
            V1_over_H->insert(v);
        }
    }

    for (auto v : G->enumerateV2()) {
        if (G->getEmbedding(v)[dim] > H) {
            V2_over_H->insert(v);
            V2_prime->insert(v);
        } else {
            V2_under_H->insert(v);
        }
    }

    if (render) H = adjustHyperplane(G, H);

    const auto flattened_CG = std::make_shared<ComparabilityBigraph>(
        std::move(V1_prime),
        std::move(V2_prime),
        G->ordering,
        dim,
        G->getPointSpaceLimit()
        );

    const auto under_H_CG = std::make_shared<ComparabilityBigraph>(
        std::move(V1_under_H),
        std::move(V2_under_H),
        G->ordering,
        G->dim,
        G->getPointSpaceLimit());


    const auto over_H_CG = std::make_shared<ComparabilityBigraph>(
        std::move(V1_over_H),
        std::move(V2_over_H),
        G->ordering,
        G->dim,
        G->getPointSpaceLimit());

    auto res = partition(flattened_CG);
    const auto part2 = partition(under_H_CG);
    const auto part3 = partition(over_H_CG);

    res.reserve(res.size() + part2.size() + part3.size());
    res.insert(res.end(), part2.begin(), part2.end());
    res.insert(res.end(), part3.begin(), part3.end());

    return res;
}
