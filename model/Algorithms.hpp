#ifndef MEMOIRE_ALGORITHMS_HPP
#define MEMOIRE_ALGORITHMS_HPP
#include "Graph.hpp"


class Algorithms {
};

class BicliquePartitioner : public Algorithms {
protected:
    double findHyperplane(const std::shared_ptr<ComparabilityBigraph>& G, unsigned dim) const;
    double adjustHyperplane(const std::shared_ptr<ComparabilityBigraph>& G, double hyperplane) const;

public:
    explicit BicliquePartitioner();
    std::vector<std::shared_ptr<ComparabilityBigraph>> partition(const std::shared_ptr<ComparabilityBigraph>& G, bool render=true) const;
};


#endif //MEMOIRE_ALGORITHMS_HPP