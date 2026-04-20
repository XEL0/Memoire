#ifndef MEMOIRE_ALGORITHMS_HPP
#define MEMOIRE_ALGORITHMS_HPP
#include "Graph.hpp"


class Algorithms {
};

class BicliquePartitioner : public Algorithms {
protected:
    double findHyperplane(std::unique_ptr<std::unordered_map<Vertex, std::vector<unsigned>>>& ordering, unsigned dim);
    double adjustHyperplane(std::unique_ptr<std::unordered_map<Vertex, std::vector<unsigned>>>& ordering, double hyperplane);

public:
    BicliquePartitioner();
    double partition(const std::shared_ptr<ComparabilityBigraph>& G, bool render=true);
};


#endif //MEMOIRE_ALGORITHMS_HPP