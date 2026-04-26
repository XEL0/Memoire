#include "Algorithms.hpp"

#include <algorithm>
#include <fstream>
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
    const std::shared_ptr<ComparabilityBigraph>& G, const bool optimize_size, const bool render) const {

    if (render) {
        json tree;
        tree["label"] = "root";
        json embedding = json::object();
        json coloring = json::object();
        for (const auto& v : G->vertices) {
            json dims = json::array();
            for (unsigned d = 0; d < G->dim; d++) {
                dims.push_back(G->getEmbeddingAt(v, d));
            }
            embedding[std::to_string(v->getId())] = dims;
        }
        for (const auto& v : G->enumerate()) {
            if (G->isInV1(v)) {
                coloring[std::to_string(v->getId())] = 0;
            } else {
                coloring[std::to_string(v->getId())] = 1;
            }
        }
        tree["coloring"] = coloring;
        tree["embedding"] = embedding;
        auto res = partition_save(G, optimize_size, tree);
        std::ofstream f("partition_tree.json");
        f << tree.dump(2);
        return res;
    }
    return partition_light(G, optimize_size);

}


std::vector<std::shared_ptr<ComparabilityBigraph>> BicliquePartitioner::partition_light(
    const std::shared_ptr<ComparabilityBigraph>& G, const bool optimize_size) const
{
    if (G->size() <= 1) return {};
    if (optimize_size and areAllV1LessThanV2(G)) return {G};
    if (G->dim == 0) return std::vector{G};

    const double H = findHyperplane(G, G->dim-1);

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

    auto res = std::vector<std::shared_ptr<ComparabilityBigraph>>{};

    if (nb_v1_under and nb_v2_over) {
        const auto flattened_CG = std::make_shared<ComparabilityBigraph>(
    std::move(V_prime), nb_v1_under, nb_v2_over, G->dim-1, G->getPointSpaceLimit());
        res = partition_light(flattened_CG, optimize_size);
    }

    if (nb_v2_under) {
        const auto under_H_CG = std::make_shared<ComparabilityBigraph>(
            std::move(V_under_H), nb_v1_under, nb_v2_under, G->dim, G->getPointSpaceLimit());
        const auto part2 = partition_light(under_H_CG, optimize_size);
        res.insert(res.end(), part2.begin(), part2.end());
    }

    if (nb_v1_over) {
        const auto over_H_CG = std::make_shared<ComparabilityBigraph>(
            std::move(V_over_H), nb_v1_over, nb_v2_over, G->dim, G->getPointSpaceLimit());
        const auto part3 = partition_light(over_H_CG, optimize_size);
        res.insert(res.end(), part3.begin(), part3.end());
    }
    return res;
}


std::vector<std::shared_ptr<ComparabilityBigraph>> BicliquePartitioner::partition_save(
    const std::shared_ptr<ComparabilityBigraph>& G, const bool optimize_size, json& node) const
{
    node["dim"]  = G->dim;
    node["size"] = G->size();
    json V = json::array();
    for (const auto& v : G->vertices) {
        V.push_back(v->getId());
    }
    node["V"] = V;
    node["children"] = json::array();

    if (G->size() <= 1) {
        return {};
    }

    std::cout << *G << std::endl;
    if (optimize_size) {
        if (areAllV1LessThanV2(G)) {
            node["optimized"] = true;
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

    H = adjustHyperplane(G, H);
    node["H"] = H;
    auto res = std::vector<std::shared_ptr<ComparabilityBigraph>>{};

    if (nb_v1_under and nb_v2_over) {
        json child;
        child["label"] = "FL";
        const auto flattened_CG = std::make_shared<ComparabilityBigraph>(
    std::move(V_prime), nb_v1_under, nb_v2_over, G->dim-1, G->getPointSpaceLimit());
        res = partition_save(flattened_CG, optimize_size, child);
        node["children"].push_back(child);
    }

    if (nb_v2_under) {
        json child;
        child["label"] = "L";
        const auto under_H_CG = std::make_shared<ComparabilityBigraph>(
            std::move(V_under_H), nb_v1_under, nb_v2_under, G->dim, G->getPointSpaceLimit());
        const auto part2 = partition_save(under_H_CG, optimize_size, child);
        node["children"].push_back(child);
        res.insert(res.end(), part2.begin(), part2.end());
    }

    if (nb_v1_over) {
        json child;
        child["label"] = "U";
        const auto over_H_CG = std::make_shared<ComparabilityBigraph>(
            std::move(V_over_H), nb_v1_over, nb_v2_over, G->dim, G->getPointSpaceLimit());
        const auto part3 = partition_save(over_H_CG, optimize_size, child);
        node["children"].push_back(child);
        res.insert(res.end(), part3.begin(), part3.end());
    }
    return res;
}

bool BicliquePartitioner::areAllV1LessThanV2(const std::shared_ptr<ComparabilityBigraph>& G) const {
    return G->isComplete();
}
