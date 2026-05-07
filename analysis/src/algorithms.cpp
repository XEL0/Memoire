#include "algorithms.hpp"

#include <iostream>

#include "Graph.hpp"


double Algorithms::findHyperplane(const std::shared_ptr<Graph>& G, unsigned dim) {
    const std::size_t n = (G->size() - 1) / 2;
    std::vector<Vertex*> sorted_vertices = G->vertices;

    std::ranges::nth_element(sorted_vertices, sorted_vertices.begin() + static_cast<std::ptrdiff_t>(n),
                             [dim](const Vertex* a, const Vertex* b) {
                                 return a->at(dim) < b->at(dim);
                             });
    const auto c = sorted_vertices[n];
    return c->at(dim);
}


std::vector<std::shared_ptr<Graph>> Algorithms::partition(
    const std::shared_ptr<Graph>& G, const bool optimize_size,
    int depth, const std::string& origin) {

    if (G->size() <= 1) return {};
    if (G->getDimension() == 0) return std::vector{G};
    //if (optimize_size and G->isPseudoBiclique()) return {G};

    const double H = findHyperplane(G, G->getDimension()-1);

    auto V_under_H = std::vector<Vertex*>();
    V_under_H.reserve(G->size()/2 + G->size()%2);
    auto V_over_H = std::vector<Vertex*>();
    V_over_H.reserve(G->size()/2);
    auto V_prime = std::vector<Vertex*>();

    unsigned nb_v1_under = 0;
    unsigned nb_v2_under = 0;
    unsigned nb_v1_over = 0;
    unsigned nb_v2_over = 0;

    for (const auto& v : G->enumerate()) {
        if (v->at(G->getDimension()-1) <= H) {
            V_under_H.push_back(v);
            if (v->isInV1()) {
                nb_v1_under++;
                V_prime.push_back(v);
            } else {
                nb_v2_under++;
            }
        } else {
            V_over_H.push_back(v);
            if (v->isInV2()) {
                nb_v2_over++;
                V_prime.push_back(v);
            } else {
                nb_v1_over++;
            }
        }
    }

    auto res = std::vector<std::shared_ptr<Graph>>{};

    if (nb_v1_under and nb_v2_over) {
        const auto flattened_CG = std::make_shared<Graph>(
            V_prime, nb_v1_under, nb_v2_over, G->getDimension()-1);
        res = partition(flattened_CG, optimize_size, depth + 1, "FL");
    }

    if (nb_v1_under and nb_v2_under) {
        const auto under_H_CG = std::make_shared<Graph>(
            V_under_H, nb_v1_under, nb_v2_under, G->getDimension());
        const auto part2 = partition(under_H_CG, optimize_size, depth + 1, "L");
        res.insert(res.end(), part2.begin(), part2.end());
    }

    if (nb_v1_over and nb_v2_over) {
        const auto over_H_CG = std::make_shared<Graph>(
            V_over_H, nb_v1_over, nb_v2_over, G->getDimension());
        const auto part3 = partition(over_H_CG, optimize_size, depth + 1, "U");
        res.insert(res.end(), part3.begin(), part3.end());
    }
    return res;
}
