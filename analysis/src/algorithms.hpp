#ifndef ANALYSIS_ALGORITHMS_HPP
#define ANALYSIS_ALGORITHMS_HPP

#include <memory>
#include "ComparabilityBigraph.hpp"


class Algorithms {
public:
    static double findHyperplane(const std::shared_ptr<ComparabilityBigraph>& G, unsigned dim);
    static std::vector<std::shared_ptr<ComparabilityBigraph>> partition(const std::shared_ptr<ComparabilityBigraph>& G, bool optimize_size, int depth, const std::string& origin = "root");
    static std::vector<std::pair<const Biclique*, unsigned>> bfs(const GraphOfBicliques& graph, unsigned u, unsigned v);
};


#endif //ANALYSIS_ALGORITHMS_HPP
