#ifndef ANALYSIS_ALGORITHMS_HPP
#define ANALYSIS_ALGORITHMS_HPP

#include <memory>
#include "Graph.hpp"


class Algorithms {
public:
    static double findHyperplane(const std::shared_ptr<Graph>& G, unsigned dim);
    static std::vector<std::shared_ptr<Graph>> partition(const std::shared_ptr<Graph>& G, bool optimize_size, int depth, const std::string& origin = "root");
};


#endif //ANALYSIS_ALGORITHMS_HPP
