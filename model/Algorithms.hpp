#ifndef MEMOIRE_ALGORITHMS_HPP
#define MEMOIRE_ALGORITHMS_HPP
#include "Graph.hpp"


class Algorithms {
};

class BicliquePartitioner : public Algorithms {
protected:
    double findHyperplane(std::vector<EmbeddedVertex>& ordering, unsigned dim);

public:
    BicliquePartitioner();
    double partition(const std::shared_ptr<ComparabilityGraph>& G);
};


#endif //MEMOIRE_ALGORITHMS_HPP