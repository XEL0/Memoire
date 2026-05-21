#include "algorithms.hpp"

#include <deque>
#include <iostream>
#include <unordered_map>

#include "ComparabilityBigraph.hpp"


double Algorithms::findHyperplane(const std::shared_ptr<ComparabilityBigraph>& G, unsigned dim) {
    const std::size_t n = (G->size() - 1) / 2;
    std::vector<Vertex*> sorted_vertices = G->vertices;

    std::ranges::nth_element(sorted_vertices, sorted_vertices.begin() + static_cast<std::ptrdiff_t>(n),
                             [dim](const Vertex* a, const Vertex* b) {
                                 return a->at(dim) < b->at(dim);
                             });
    const auto c = sorted_vertices[n];
    return c->at(dim);
}


std::vector<std::shared_ptr<ComparabilityBigraph>> Algorithms::partition(
    const std::shared_ptr<ComparabilityBigraph>& G, const bool optimize_size,
    int depth, const std::string& origin) {

    if (G->size() <= 1) return {};
    if (G->getDimension() == 0) return std::vector{G};
    if (optimize_size and G->isComplete()) return {G};

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

    auto res = std::vector<std::shared_ptr<ComparabilityBigraph>>{};

    if (nb_v1_under and nb_v2_over) {
        const auto flattened_CG = std::make_shared<ComparabilityBigraph>(
            V_prime, nb_v1_under, nb_v2_over, G->getDimension()-1);
        res = partition(flattened_CG, optimize_size, depth + 1, "FL");
    }

    if (nb_v1_under and nb_v2_under) {
        const auto under_H_CG = std::make_shared<ComparabilityBigraph>(
            V_under_H, nb_v1_under, nb_v2_under, G->getDimension());
        const auto part2 = partition(under_H_CG, optimize_size, depth + 1, "L");
        res.insert(res.end(), part2.begin(), part2.end());
    }

    if (nb_v1_over and nb_v2_over) {
        const auto over_H_CG = std::make_shared<ComparabilityBigraph>(
            V_over_H, nb_v1_over, nb_v2_over, G->getDimension());
        const auto part3 = partition(over_H_CG, optimize_size, depth + 1, "U");
        res.insert(res.end(), part3.begin(), part3.end());
    }
    return res;
}

std::vector<std::pair<const Biclique*, unsigned>> Algorithms::bfs(const GraphOfBicliques& graph, const unsigned u, const unsigned v) {
    if (u == v) return {};
    std::deque<const Biclique*> bfs_queue;
    std::unordered_set<const Biclique*> visited = {};
    std::unordered_map<const Biclique*, std::vector<std::tuple<const Biclique*, unsigned, int>>> parents = {};
    unsigned color;
    unsigned depth = 0;

    for (const Biclique& biclique : graph.bicliques) {
        if (biclique.contains(u)) {
            color = biclique.containsP(u) ? 0 : 1;
            bfs_queue.push_back(&biclique);
            parents[&biclique].emplace_back(nullptr, u, -1);
            if (color == 0 and biclique.p() > 1 or color == 1 and biclique.q() > 1) continue;
            visited.insert(&biclique);
        }
    }
    if (bfs_queue.empty()) return {};

    unsigned depth_size = bfs_queue.size();
    while (not bfs_queue.empty() and not bfs_queue.front()->contains(v)) {
        const Biclique* current = bfs_queue.front();
        bfs_queue.pop_front();

        if (color == 0) {
            for (const auto q : current->getV2()) {
                for (const Biclique& biclique : graph.bicliques) {
                    if (biclique.containsQ(q) and not visited.contains(&biclique)) {
                        bfs_queue.push_back(&biclique);
                        visited.insert(&biclique);
                        parents[&biclique].emplace_back(current, q, depth);
                    }
                }
            }
        } else {
            for (const auto p : current->getV1()) {
                for (const Biclique& biclique : graph.bicliques) {
                    if (biclique.containsP(p) and not visited.contains(&biclique)) {
                        bfs_queue.push_back(&biclique);
                        visited.insert(&biclique);
                        parents[&biclique].emplace_back(current, p, depth);
                    }
                }
            }
        }
        depth_size -= 1;
        if (depth_size == 0) {
            depth += 1;
            depth_size = bfs_queue.size();
            color = !color;
        }
    }
    if (bfs_queue.empty()) return {};

    std::vector<std::pair<const Biclique*, unsigned>> path;
    const Biclique* end = bfs_queue.front();
    path.emplace_back(end, v);

    while (end != nullptr) {
        auto x = parents[end];
        auto it = std::ranges::find_if(x, [&](const auto& e) { return std::get<2>(e) == depth-1; });
        if (it == x.end()) break;

        auto prev = std::get<0>(*it);
        unsigned vertex = std::get<1>(*it);

        if (prev == nullptr) break;
        path.emplace_back(prev, vertex);
        end = prev;
        depth -= 1;
    }
    std::ranges::reverse(path);

    return path;
}