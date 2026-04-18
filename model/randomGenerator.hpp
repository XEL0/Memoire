#ifndef MEMOIRE_RANDOMGENERATOR_HPP
#define MEMOIRE_RANDOMGENERATOR_HPP

#include <random>

class RandomGenerator {
protected:
    std::random_device dev;
    std::mt19937 rng;
    std::uniform_int_distribution<std::mt19937::result_type> dist;
public:
    RandomGenerator(const unsigned lower, const unsigned upper): rng(dev()), dist(lower, upper) {}

    unsigned operator() (){
        return dist(rng);
    }

    std::mt19937& getRNG() { return rng; }
};

#endif //MEMOIRE_RANDOMGENERATOR_HPP