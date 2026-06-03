#ifndef MEMOIRE_RANDOMGENERATOR_HPP
#define MEMOIRE_RANDOMGENERATOR_HPP

#include <random>

class RandomGenerator {
protected:
    std::mt19937 rng;
    std::uniform_int_distribution<std::mt19937::result_type> dist;
    unsigned seed;
public:
    RandomGenerator(const unsigned lower, const unsigned upper, const unsigned seed = 0):
        rng(seed), dist(lower, upper), seed(seed) {}

    unsigned operator() (){
        return dist(rng);
    }

    std::mt19937& getRNG() { return rng; }
    unsigned getSeed() const { return seed; }

    void setSeed(const unsigned new_seed) {
        seed = new_seed;
        rng.seed(seed);
    }
};

#endif //MEMOIRE_RANDOMGENERATOR_HPP