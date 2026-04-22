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
        auto pos = G->getEmbeddingAt(v, G->dim - 1);
        if (const double dist = pos - hyperplane; dist < minDistance and dist > 0) {
            minDistance = dist;
            next = pos;
        }
    }
    return (hyperplane + next) / 2;
}


std::vector<std::shared_ptr<ComparabilityBigraph>> BicliquePartitioner::partition(const std::shared_ptr<ComparabilityBigraph>& G, const bool render) const {

    if (G->dim == 0) {
        return std::vector{G};
    }
    const double H = findHyperplane(G, G->dim-1);
    std::cout << "G = " << *G << std::endl;
    std::cout << "H = " << H << std::endl;

    auto V_under_H = std::vector<VertexPointer>{};
    auto V_over_H = std::vector<VertexPointer>{};
    auto V_prime = std::vector<VertexPointer>{};

    bool V1_empty_under = true;
    bool V2_empty_under = true;
    bool V1_empty_over = true;
    bool V2_empty_over = true;

    for (const auto& v : G->enumerate()) {
        if (G->getEmbeddingAt(v, G->dim-1) <= H) {
            V_under_H.push_back(v);
            if (G->isInV1(v)) {
                V1_empty_under = false;
                V_prime.push_back(v);
            } else { V2_empty_under = false; }

        } else {
            V_over_H.push_back(v);
            if (G->isInV2(v)) {
                V2_empty_over = false;
                V_prime.push_back(v);
            } else { V1_empty_over = false; }
        }
    }

    //if (render) H = adjustHyperplane(G, H);

    if (V1_empty_under or V2_empty_over) return std::vector<std::shared_ptr<ComparabilityBigraph>>{};

    const auto flattened_CG = std::make_shared<ComparabilityBigraph>(
        std::move(V_prime), G->dim-1, G->getPointSpaceLimit());
    auto res = partition(flattened_CG);

    if (not V2_empty_under) {
        const auto under_H_CG = std::make_shared<ComparabilityBigraph>(
            std::move(V_under_H), G->dim, G->getPointSpaceLimit());
        const auto part2 = partition(under_H_CG);
        res.insert(res.end(), part2.begin(), part2.end());
    }

    if (not V1_empty_over) {
        const auto over_H_CG = std::make_shared<ComparabilityBigraph>(
            std::move(V_over_H), G->dim, G->getPointSpaceLimit());
        const auto part3 = partition(over_H_CG);
        res.insert(res.end(), part3.begin(), part3.end());
    }
    return res;
}

std::vector<std::shared_ptr<ComparabilityBigraph>> BicliquePartitioner::partitionBigger(const std::shared_ptr<ComparabilityBigraph>& G, const bool render) const {
    if (G->dim == 1) {
        bool red = true;
        auto V_final = std::vector<VertexPointer>{};
        for (auto v : G->enumerate()) {
            if ( not red or G->isInV1(v)) {
                red = false;
                V_final.push_back(v);
            }
        }
        for (auto v : G->reverseEnumerate()) {
            if (G->isInV1(v)) {
                V_final.pop_back();
                if (G->isInV2(V_final.back())) break;
            }
        }
        auto G_new = std::make_shared<ComparabilityBigraph>(V_final, 0, G->getPointSpaceLimit());
        return std::vector{G_new};
    }
    const double H = findHyperplane(G, G->dim-1);
    std::cout << "G = " << *G << std::endl;
    std::cout << "H = " << H << std::endl;

    auto V_under_H = std::vector<VertexPointer>{};
    auto V_over_H = std::vector<VertexPointer>{};
    auto V_prime = std::vector<VertexPointer>{};

    bool V1_empty_under = true;
    bool V2_empty_under = true;
    bool V1_empty_over = true;
    bool V2_empty_over = true;

    for (const auto& v : G->enumerate()) {
        if (G->getEmbeddingAt(v, G->dim-1) <= H) {
            V_under_H.push_back(v);
            if (G->isInV1(v)) {
                V1_empty_under = false;
                V_prime.push_back(v);
            } else { V2_empty_under = false; }

        } else {
            V_over_H.push_back(v);
            if (G->isInV2(v)) {
                V2_empty_over = false;
                V_prime.push_back(v);
            } else { V1_empty_over = false; }
        }
    }

    //if (render) H = adjustHyperplane(G, H);

    if (V1_empty_under or V2_empty_over) return std::vector<std::shared_ptr<ComparabilityBigraph>>{};

    const auto flattened_CG = std::make_shared<ComparabilityBigraph>(
        std::move(V_prime), G->dim-1, G->getPointSpaceLimit());
    auto res = partition(flattened_CG);

    if (not V2_empty_under) {
        const auto under_H_CG = std::make_shared<ComparabilityBigraph>(
            std::move(V_under_H), G->dim, G->getPointSpaceLimit());
        const auto part2 = partition(under_H_CG);
        res.insert(res.end(), part2.begin(), part2.end());
    }

    if (not V1_empty_over) {
        const auto over_H_CG = std::make_shared<ComparabilityBigraph>(
            std::move(V_over_H), G->dim, G->getPointSpaceLimit());
        const auto part3 = partition(over_H_CG);
        res.insert(res.end(), part3.begin(), part3.end());
    }
    return res;
}
