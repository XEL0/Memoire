#include <memory>
#include <iostream>

#include "ComparabilityBigraph.hpp"
#include "algorithms.hpp"

int main(int argc, char *argv[]) {
    /*if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " p q dim" << std::endl;
        return 1;
    }
    unsigned p = std::stoi(argv[1]), q = std::stoi(argv[2]), dim = std::stoi(argv[3]);*/
    unsigned p = 20, q = 20, dim = 2;
    auto gg = new GraphGenerator();
    //const auto vertices = gg->generate(p, q, dim);

    /*std::cout << "std::vector<VertexPointer> V{" << std::endl;
    for (const auto& v : vertices) {
        std::cout << "\tstd::make_shared<ColoredEmbeddedVertex>(" << v->getId() << ", " << v->isInV2() << ", std::vector<unsigned>{" << v->getEmbedding()[0] << ", " << v->getEmbedding()[1] << "})," << std::endl;
    }
    std::cout << "};" << std::endl;
    std::cout << "auto graph = std::make_shared<ComparabilityBigraph>(std::move(V), " << p << ", " << q << ", " << dim << ");" << std::endl;*/


    /*std::cout << "const std::vector<Vertex*>& generatePreset3() {" << std::endl;
    for (const auto& v : vertices) {
        std::cout << "\tvertices.push_back(new Vertex(" << v->getId() << ", " << v->isInV2() << ", std::vector<unsigned>{" << v->getEmbedding()[0] << ", " << v->getEmbedding()[1] << "}));" << std::endl;
    }
    std::cout << "\treturn vertices;\n}" << std::endl;*/



    const auto vertices = gg->generatePreset1();
    const auto CBg = std::make_shared<ComparabilityBigraph>(vertices, p, q, dim);
    const auto x = Algorithms::partition(CBg, true, 0);

    std::vector<Biclique> bicliques;
    for (const auto& G : x) {
        bicliques.push_back(G->toBiclique());
        std::cout << *G << std::endl;
    }
    const auto graph = GraphOfBicliques(bicliques, 0, 0);


    std::vector<Biclique> bicliques2;
    Biclique b1 = {std::unordered_set<unsigned>{3}, std::unordered_set<unsigned>{8}};
    Biclique b2 = {std::unordered_set<unsigned>{6}, std::unordered_set<unsigned>{8}};
    Biclique b3 = {std::unordered_set<unsigned>{8}, std::unordered_set<unsigned>{5,2}};
    Biclique b4 = {std::unordered_set<unsigned>{6}, std::unordered_set<unsigned>{0, 9}};
    Biclique b5 = {std::unordered_set<unsigned>{2}, std::unordered_set<unsigned>{3}};
    bicliques2.push_back(b1);
    bicliques2.push_back(b2);
    bicliques2.push_back(b3);
    bicliques2.push_back(b4);
    bicliques2.push_back(b5);
    const auto graph2 = GraphOfBicliques(bicliques2, 0, 0);

    auto path = Algorithms::bfs(graph2, 2, 0);
    std::cout << "Path length = " << path.size() << std::endl;
    std::cout << "Path: " << std::endl;
    for (const auto& [biclique, vertex] : path) {
        std::cout << *biclique << " " << vertex << std::endl;
    }

    delete gg;
    return 0;
}
