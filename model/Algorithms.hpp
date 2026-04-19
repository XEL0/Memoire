#ifndef MEMOIRE_ALGORITHMS_HPP
#define MEMOIRE_ALGORITHMS_HPP
#include "Graph.hpp"


class Algorithms {
};

class BicliquePartitioner : public Algorithms {
protected:
    double findHyperplane(std::vector<EmbeddedVertex>& ordering, unsigned dim);
    double adjustHyperplane(std::vector<EmbeddedVertex>& ordering, double hyperplane);
public:
    BicliquePartitioner();
    double partition(const std::shared_ptr<ComparabilityGraph>& G, bool render=true);
};


#endif //MEMOIRE_ALGORITHMS_HPP