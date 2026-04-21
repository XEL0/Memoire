#include "Algorithms.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>

BicliquePartitioner::BicliquePartitioner() = default;



auto to_vector(std::shared_ptr<std::unordered_map<Vertex, std::vector<unsigned>>>& map) {
    std::vector<std::vector<unsigned>*> items;
    items.reserve(map->size());

    for (auto &[v, pos] : *map) {
        items.push_back(&pos);
    }
    return items;
}

double BicliquePartitioner::findHyperplane(const std::shared_ptr<ComparabilityBigraph>& G, unsigned dim) const {
    std::cout << "b0" << std::endl;
    const std::size_t n = (G->size() - 1) / 2;
    std:: cout << "b0.1" << std::endl;
    std::cout << G->size() << std::endl;
    std::cout << "n: " << n << std::endl;

    assert(G);
    assert(G->V1);
    assert(G->V2);
    std::cout << "V1 size = " << G->V1->size() << '\n';
    std::cout << "V2 size = " << G->V2->size() << '\n';

    std::cout << "V1 ptr = " << G->V1.get() << '\n';
    std::cout << "V2 ptr = " << G->V2.get() << '\n';
    std::vector<Vertex> V;
    std::cout << "b0.2" << std::endl;

    std::cout << "enumerateV1 begin" << std::endl;
    auto r1 = G->enumerateV1();
    std::cout << "enumerateV1 ok" << std::endl;

    for (auto v : G->enumerateV1()) { V.push_back(v); }
    std::cout << "b0.3" << std::endl;
    for (auto v : G->enumerateV2()) {V.push_back(v);}
    std::cout << "b1" << std::endl;

    std::ranges::nth_element(V, V.begin() + static_cast<std::ptrdiff_t>(n),
                             [dim, G](const Vertex a, const Vertex b) {
                                 return G->ordering->at(a)[dim] < G->ordering->at(b)[dim];
                             });

    std::cout << "b2" << std::endl;

    return G->ordering->at(n)[dim];
}

double BicliquePartitioner::adjustHyperplane(const std::shared_ptr<ComparabilityBigraph>& G, const double hyperplane) const {
    double minDistance = std::numeric_limits<double>::infinity();
    double next = hyperplane;

    for (const auto& [v, pos] : *G->ordering) {
        if (const double dist = pos[1] - hyperplane; dist < minDistance and dist > 0) {
            minDistance = dist;
            next = pos[1];
        }
    }
    return (hyperplane + next) / 2;
}


std::vector<std::shared_ptr<ComparabilityBigraph>> BicliquePartitioner::partition(const std::shared_ptr<ComparabilityBigraph>& G, const bool render) const {
    std::cout << "a0 " << G << std::endl;
    if (G->size() < 2) {
        return std::vector<std::shared_ptr<ComparabilityBigraph>>{};
    }
    std::cout << "a1" << std::endl;
    if (G->dim == 0) {
        // return std::vector{G};
        return (G->enumerateV1().empty() or G->enumerateV2().empty()) ? std::vector<std::shared_ptr<ComparabilityBigraph>>{} : std::vector{G};
    }

    std::cout << "a2" << std::endl;
    const auto dim = G->dim - 1;
    double H = findHyperplane(G, dim);

    auto V1_under_H = std::make_unique<std::unordered_set<Vertex>>();
    auto V2_under_H = std::make_unique<std::unordered_set<Vertex>>();

    auto V1_over_H = std::make_unique<std::unordered_set<Vertex>>();
    auto V2_over_H = std::make_unique<std::unordered_set<Vertex>>();

    auto V1_prime = std::make_unique<std::unordered_set<Vertex>>();
    auto V2_prime = std::make_unique<std::unordered_set<Vertex>>();

    std::cout << "a3" << std::endl;
    for (auto v : G->enumerateV1()) {
        if (G->ordering->at(v)[dim] <= H) {
            V1_under_H->insert(v);
            V1_prime->insert(v);
        } else {
            V1_over_H->insert(v);
        }
    }

    std::cout << "a4" << std::endl;
    for (auto v : G->enumerateV2()) {
        if (G->ordering->at(v)[dim] > H) {
            V2_over_H->insert(v);
            V2_prime->insert(v);
        } else {
            V2_under_H->insert(v);
        }
    }

    /*std::ranges::copy_if(
        G->enumerateV1(),
        std::inserter(*V1_prime, V1_prime->end()),
        [H, this, dim](const Vertex vertex) {
            return G->ordering->at(vertex)[dim] <= H;
        }
        );

    std::ranges::copy_if(
        G->enumerateV2(),
        std::inserter(*V2_prime, V2_prime->end()),
        [H, this, dim](const Vertex vertex) {
            return G->ordering->at(vertex)[dim] > H;
        }
        );*/

    std::cout << "a4.1" << std::endl;

    if (render) H = adjustHyperplane(G, H);

    std::cout << "a4.5" << std::endl;

    const auto flattened_CG = std::make_shared<ComparabilityBigraph>(
        std::move(V1_prime),
        std::move(V2_prime),
        std::unique_ptr<std::vector<Edge>>{},
        G->ordering,
        dim - 1,
        G->getPointSpaceLimit()
        );

    std::cout << "a4.6" << std::endl;

    const auto under_H_CG = std::make_shared<ComparabilityBigraph>(
        std::move(V1_under_H),
        std::move(V2_under_H),
        std::unique_ptr<std::vector<Edge>>{},
        G->ordering,
        G->dim,
        G->getPointSpaceLimit());

    std::cout << "a4.7" << std::endl;

    const auto over_H_CG = std::make_shared<ComparabilityBigraph>(
        std::move(V1_over_H),
        std::move(V2_over_H),
        std::unique_ptr<std::vector<Edge>>{},
        G->ordering,
        G->dim,
        G->getPointSpaceLimit());

    std::cout << "a5" << std::endl;

    auto res = partition(flattened_CG);
    const auto part2 = partition(under_H_CG);
    const auto part3 = partition(over_H_CG);

    res.reserve(res.size() + part2.size() + part3.size());
    res.insert(res.end(), part2.begin(), part2.end());
    res.insert(res.end(), part3.begin(), part3.end());

    std::cout << "a6" << std::endl;
    return res;
}
