#include "Algorithms.hpp"
#include "Bigraph.hpp"
#include <algorithm>
#include <deque>
#include <fstream>
#include <unordered_set>
#include <iostream>

BicliquePartitioner::BicliquePartitioner() = default;


double BicliquePartitioner::findHyperplane(const std::shared_ptr<ComparabilityBigraph>& G, unsigned dim) {
    const std::size_t n = (G->size() - 1) / 2;
    std::ranges::nth_element(G->vertices, G->vertices.begin() + static_cast<std::ptrdiff_t>(n),
                             [dim, G](const VertexPointer& a, const VertexPointer& b) {
                                 return G->getEmbeddingAt(a, dim) < G->getEmbeddingAt(b, dim);
                             });
    const auto at = G->vertices.begin() + static_cast<std::ptrdiff_t>(n);
    return G->getEmbeddingAt(*at, dim);
}

double BicliquePartitioner::adjustHyperplane(const std::shared_ptr<ComparabilityBigraph>& G, const double hyperplane) {
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
    const std::shared_ptr<ComparabilityBigraph>& G, const bool optimize_size) {
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
    tree["point_space_limit"] = std::to_string(G->getPointSpaceLimit());
    auto res = partition_save(G, optimize_size, tree);
    std::ofstream f("partition_tree.json");
    f << tree.dump(2);
    return res;

}


std::vector<std::shared_ptr<ComparabilityBigraph>> BicliquePartitioner::partition_save(
    const std::shared_ptr<ComparabilityBigraph>& G, const bool optimize_size, json& node)
{
    node["dim"]  = G->dim;
    json V = json::array();
    for (const auto& v : G->vertices) {
        V.push_back(v->getId());
    }
    node["V"] = V;
    if (!node.contains("children")) node["children"] = json::array();

    if (G->size() <= 1) {
        return {};
    }

    if (G->dim == 0) {
        return std::vector{G};
    }

    if (optimize_size) {
        if (areAllV1LessThanV2(G)) {
            node["optimized"] = true;
            return {G};
        }
    }

    double H = findHyperplane(G, G->dim-1);

    auto V_under_H = std::make_unique<std::vector<VertexPointer>>();
    auto V_over_H = std::make_unique<std::vector<VertexPointer>>();
    auto V_prime = std::make_unique<std::vector<VertexPointer>>();
    V_under_H->reserve(G->size()/2 + 1);
    V_over_H->reserve(G->size()/2);

    unsigned nb_v1_under = 0;
    unsigned nb_v2_under = 0;
    unsigned nb_v1_over = 0;
    unsigned nb_v2_over = 0;

    for (const auto& v : G->enumerate()) {
        if (G->getEmbeddingAt(v, G->dim-1) <= H) {
            V_under_H->push_back(v);
            if (G->isInV1(v)) {
                nb_v1_under++;
                V_prime->push_back(v);
            } else {
                nb_v2_under++;
            }

        } else {
            V_over_H->push_back(v);
            if (G->isInV2(v)) {
                nb_v2_over++;
                V_prime->push_back(v);
            } else {
                nb_v1_over++;
            }
        }
    }

    H = adjustHyperplane(G, H);
    node["H"] = H;
    auto res = std::vector<std::shared_ptr<ComparabilityBigraph>>{};
    node["FL"] = nb_v1_under and nb_v2_over;
    node["L"] = nb_v1_under and nb_v2_under;
    node["U"] = nb_v1_over and nb_v2_over;

    if (nb_v1_under and nb_v2_over) {
        json child = json::object();
        child["label"] = "FL";
        const auto flattened_CG = std::make_shared<ComparabilityBigraph>(
            std::move(*V_prime), nb_v1_under, nb_v2_over, G->dim-1, G->getPointSpaceLimit());
        res = partition_save(flattened_CG, optimize_size, child);
        node["children"].push_back(std::move(child));
    }

    if (nb_v1_under and nb_v2_under) {
        json child = json::object();
        child["label"] = "L";
        const auto under_H_CG = std::make_shared<ComparabilityBigraph>(
            std::move(*V_under_H), nb_v1_under, nb_v2_under, G->dim, G->getPointSpaceLimit());
        const auto part2 = partition_save(under_H_CG, optimize_size, child);
        node["children"].push_back(std::move(child));
        res.insert(res.end(), part2.begin(), part2.end());
    }

    if (nb_v1_over and nb_v2_over) {
        json child = json::object();
        child["label"] = "U";
        const auto over_H_CG = std::make_shared<ComparabilityBigraph>(
            std::move(*V_over_H), nb_v1_over, nb_v2_over, G->dim, G->getPointSpaceLimit());
        const auto part3 = partition_save(over_H_CG, optimize_size, child);
        node["children"].push_back(std::move(child));
        res.insert(res.end(), part3.begin(), part3.end());
    }
    return res;
}

bool BicliquePartitioner::areAllV1LessThanV2(const std::shared_ptr<ComparabilityBigraph>& G) {
    return G->isComplete(); //todo find first blue and last red; check completeness in between
}





std::vector<std::shared_ptr<ComparabilityBigraph>> CappedGraphDecomposition::decompose(
    const std::shared_ptr<TerrainVisibilityGraph>& G) {
    return decomposeRecursive(G);
}

std::vector<std::shared_ptr<ComparabilityBigraph>> CappedGraphDecomposition::decomposeRecursive(
    const std::shared_ptr<TerrainVisibilityGraph>& G) {

    const unsigned n = G->size();

    if (n <= 1) return {};

    const unsigned half = n / 2;
    std::vector<VertexPointer> V1;
    std::vector<VertexPointer> V2;
    V1.reserve(half);
    V2.reserve(n - half);

    unsigned i = 0;
    unsigned max_id_V1 = 0;
    for (const auto& v : G->enumerate()) {
        if (i < half) {
            V1.push_back(v);
            max_id_V1 = v->getId();
        } else {
            V2.push_back(v);
        }
        i++;
    }

    std::vector<Edge> edges_H;
    for (const auto& edge : G->enumerate_edges()) {
        const bool u_in_V1 = edge.first->getId() <= max_id_V1;
        const bool v_in_V1 = edge.second->getId() <= max_id_V1;
        if (u_in_V1 != v_in_V1) edges_H.push_back(edge);
    }
    const auto H_comparability = convertToComparabilityBigraph(V1, V2, edges_H);
    std::vector<std::shared_ptr<ComparabilityBigraph>> result;

    if (H_comparability->size() > 1) {
        result = BicliquePartitioner::partition(H_comparability, false);
    }
    if (V1.size() > 1) {
        std::vector<Edge> edges_V1;
        for (const auto& edge : G->enumerate_edges()) {
            const bool u_in_V1 = edge.first->getId() <= max_id_V1;
            const bool v_in_V1 = edge.second->getId() <= max_id_V1;
            if (u_in_V1 and v_in_V1) edges_V1.push_back(edge);

        }
        const auto G_V1 = std::make_shared<TerrainVisibilityGraph>(V1, edges_V1);
        auto decomposed_V2 = decomposeRecursive(G_V1);
        result.insert(result.end(), decomposed_V2.begin(), decomposed_V2.end());
    }
    if (V2.size() > 1) {
        std::vector<Edge> edges_V2;
        for (const auto& edge : G->enumerate_edges()) {
            const bool u_in_V2 = edge.first->getId() > max_id_V1;
            const bool v_in_V2 = edge.second->getId() > max_id_V1;
            if (u_in_V2 and v_in_V2) edges_V2.push_back(edge);
        }
        const auto G_V2 = std::make_shared<TerrainVisibilityGraph>(V2, edges_V2);
        auto decomposed_V2 = decomposeRecursive(G_V2);
        result.insert(result.end(), decomposed_V2.begin(), decomposed_V2.end());
    }
    return result;
}

std::shared_ptr<ComparabilityBigraph> CappedGraphDecomposition::convertToComparabilityBigraph(
    const std::vector<VertexPointer>& V1, const std::vector<VertexPointer>& V2, const std::vector<Edge>& edges_H) {

    const unsigned p = V1.size();
    const unsigned q = V2.size();
    const unsigned n = p + q;
    if (n == 0) return std::make_shared<ComparabilityBigraph>();

    std::unordered_set<unsigned> non_isolated_vertices;
    for (const auto& [u, v] : edges_H) {
        non_isolated_vertices.insert(u->getId());
        non_isolated_vertices.insert(v->getId());
    }

    std::vector<VertexPointer> new_vertices;
    new_vertices.reserve(n);

    std::unordered_map<unsigned, unsigned> coord_usage_dim0;
    std::unordered_map<unsigned, unsigned> coord_usage_dim1;
    constexpr unsigned EPSILON = 1;
    constexpr unsigned SCALE = 1000;

    auto getUniqueCoord = [&](const unsigned base_coord, const unsigned dim) -> unsigned {
        auto& coord_usage = (dim == 0) ? coord_usage_dim0 : coord_usage_dim1;
        const unsigned multiplier = coord_usage[base_coord];
        coord_usage[base_coord]++;
        return base_coord + multiplier * EPSILON;
    };

    unsigned p_prime = 0;
    unsigned q_prime = 0;

    auto min_max = findMinAndMaxNeighborhood(V1, V2, edges_H);

    for (unsigned i = 0; i < p; i++) {
        const auto& v = V1[i];
        unsigned id = v->getId();
        if (not non_isolated_vertices.contains(id)) continue;
        p_prime++;
        //const unsigned min_neighbor = findMinNeighbor(v, edges_H);
        const unsigned x = getUniqueCoord(id * SCALE, 0);
        const unsigned y = getUniqueCoord(min_max[id].first * SCALE - SCALE / 2, 1);
        std::vector embedding = {x, y};
        auto new_v = std::make_shared<ColoredEmbeddedVertex>(id, 0, embedding);
        new_vertices.push_back(new_v);
    }

    for (unsigned i = 0; i < q; i++) {
        const auto& v = V2[i];
        unsigned id = v->getId();
        if (not non_isolated_vertices.contains(id)) continue;
        q_prime++;
        //const unsigned max_neighbor = findMaxNeighbor(v, edges_H);
        const unsigned x = getUniqueCoord(min_max[id].second * SCALE + SCALE / 2, 0);
        const unsigned y = getUniqueCoord(id * SCALE, 1);
        std::vector embedding = {x, y};
        auto new_v = std::make_shared<ColoredEmbeddedVertex>(id, 1, embedding);
        new_vertices.push_back(new_v);
    }

    if (new_vertices.empty()) return std::make_shared<ComparabilityBigraph>();

    /*unsigned point_space_limit = 0;
    for (const auto& v : new_vertices) {
        const auto ev = std::dynamic_pointer_cast<EmbeddedVertex>(v);
        for (unsigned d = 0; d < 2; d++) {
            point_space_limit = std::max(point_space_limit, ev->at(d));
        }
    }*/

    auto result = std::make_shared<ComparabilityBigraph>(new_vertices, p_prime, q_prime, 2, (n+1) * SCALE );
    result->constructE(true);
    return result;
}

unsigned CappedGraphDecomposition::findMinNeighbor(const VertexPointer& v, const std::vector<Edge>& edges) {
    unsigned min_id = std::numeric_limits<unsigned>::max();
    const unsigned v_id = v->getId();

    for (const auto&[u, w] : edges) {
        if (u->getId() == v_id) {
            min_id = std::min(min_id, w->getId());
        } else if (w->getId() == v_id) {
            min_id = std::min(min_id, u->getId());
        }
    }
    return min_id;
}

unsigned CappedGraphDecomposition::findMaxNeighbor(const VertexPointer& v, const std::vector<Edge>& edges) {
    unsigned max_id = 0;
    const unsigned v_id = v->getId();

    for (const auto&[u, w] : edges) {
        if (u->getId() == v_id) {
            max_id = std::max(max_id, w->getId());
        } else if (w->getId() == v_id) {
            max_id = std::max(max_id, u->getId());
        }
    }
    return max_id;
}

std::unordered_map<unsigned, std::pair<unsigned, unsigned>> CappedGraphDecomposition::findMinAndMaxNeighborhood(
    const std::vector<VertexPointer>& V1, const std::vector<VertexPointer>& V2, const std::vector<Edge>& edges) {
    std::unordered_map<unsigned, std::pair<unsigned, unsigned>> neighbors;

    // .first = min_neighbor, .second = max_neighbor
    for (const auto& v : V1) {
        neighbors[v->getId()].first = std::numeric_limits<unsigned>::max();
        neighbors[v->getId()].second = 0;
    }
    for (const auto& v : V2) {
        neighbors[v->getId()].first = std::numeric_limits<unsigned>::max();
        neighbors[v->getId()].second = 0;
    }

    for (const auto& [u, v] : edges) {
        unsigned u_id = u->getId();
        unsigned v_id = v->getId();
        neighbors[u_id].first = std::min(neighbors[u_id].first, v_id);
        neighbors[u_id].second = std::max(neighbors[u_id].second, v_id);
        neighbors[v_id].first = std::min(neighbors[v_id].first, u_id);
        neighbors[v_id].second = std::max(neighbors[v_id].second, u_id);
    }

    return neighbors;
}


std::vector<std::pair<const Biclique*, unsigned>> Algorithms::bfs(const GraphOfBicliques& graph, const unsigned u, const unsigned v) {
    if (u == v) return {};

    struct BfsNode {
        const Biclique* biclique;
        unsigned vertex;
    };

    std::deque<BfsNode> bfs_queue;
    std::unordered_set<const Biclique*> visited = {};
    std::unordered_map<const Biclique*, std::vector<std::tuple<const Biclique*, unsigned, int>>> parents = {};
    unsigned depth = 0;

    for (const Biclique& biclique : graph.bicliques) {
        if (biclique.contains(u)) {
            bfs_queue.push_back({&biclique, u});
            parents[&biclique].emplace_back(nullptr, u, -1);
            if ((biclique.containsP(u) and biclique.p() > 1) or (biclique.containsQ(u) and biclique.q() > 1)) continue;
            visited.insert(&biclique);
        }
    }
    if (bfs_queue.empty()) return {};

    unsigned depth_size = bfs_queue.size();

    while (not bfs_queue.empty() and not bfs_queue.front().biclique->contains(v)) {
        auto [current, from] = bfs_queue.front();
        bfs_queue.pop_front();

        std::cout << *current << std::endl;
        auto neighbors = current->containsP(from) ? current->getV2() : current->getV1();
        for (const auto neigh : neighbors) {
            for (const Biclique& biclique : graph.bicliques) {
                if (biclique.contains(neigh)) {
                    if (not visited.contains(&biclique)) {
                        bfs_queue.push_back({&biclique, neigh});
                        visited.insert(&biclique);
                        parents[&biclique].emplace_back(current, from, depth);
                    }
                }
            }
        }
        depth_size -= 1;
        if (depth_size == 0) {
            depth += 1;
            depth_size = bfs_queue.size();
        }
    }
    if (bfs_queue.empty()) return {};

    std::vector<std::pair<const Biclique*, unsigned>> path;
    const Biclique* end = bfs_queue.front().biclique;
    unsigned end_vertex = bfs_queue.front().vertex;
    path.emplace_back(end, end_vertex);


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