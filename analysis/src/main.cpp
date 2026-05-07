#include <memory>
#include <iostream>

#include "Graph.hpp"
#include "algorithms.hpp"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " p q dim" << std::endl;
        return 1;
    }
    unsigned p = std::stoi(argv[1]), q = std::stoi(argv[2]), dim = std::stoi(argv[3]);
    auto gg = new GraphGenerator();
    const auto vertices = gg->generate(p, q, dim);
    //const auto vertices = gg.generatePreset1();
    const auto graph = std::make_shared<Graph>(vertices, p, q, dim);
    auto x = Algorithms::partition(graph, false, 0);
    for (const auto& G : x) {
        std::cout << *G << std::endl;
    }
    delete gg;
    return 0;
}
