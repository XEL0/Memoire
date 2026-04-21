#include <iostream>
#include <QApplication>

#include "model/Algorithms.hpp"
#include "model/Graph.hpp"
#include "view/DrawableGraph.hpp"


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    unsigned width = 900;
    unsigned height = 900;
    unsigned size = 900;

    QWidget window;
    window.setWindowTitle("Visualiser");
    window.resize(width, height);

    QPalette palette = window.palette();
    palette.setColor(QPalette::Window, QColor(250, 248, 240));
    window.setAutoFillBackground(true);
    window.setPalette(palette);

    auto a = std::unordered_set<Vertex>{1, 2, 3};
    auto b = std::unordered_set<Vertex>{0, 4, 5, 6, 7};
    std::unique_ptr<std::unordered_set<Vertex>> V1 = std::make_unique<std::unordered_set<Vertex>>(a);
    std::unique_ptr<std::unordered_set<Vertex>> V2 = std::make_unique<std::unordered_set<Vertex>>(b);
    std::shared_ptr<std::unordered_map<Vertex, std::vector<unsigned>>> ordering = std::make_shared<std::unordered_map<Vertex, std::vector<unsigned>>>();
    ordering->insert({1, {100, 200}});
    ordering->insert({2, {200, 400}});
    ordering->insert({3, {300, 300}});
    ordering->insert({0, {0, 0}});
    ordering->insert({4, {400, 500}});
    ordering->insert({5, {500, 100}});
    ordering->insert({6, {600, 600}});
    ordering->insert({7, {700, 700}});

    auto graph = std::make_shared<ComparabilityBigraph>(std::move(V1), std::move(V2), ordering, 2, 900);

    DrawableComparabilityBigraph drawableGraph(&window);

    /*auto graph = std::make_shared<ComparabilityBigraph>();
    graph->generate(5, 5, 2, 900);
    DrawableComparabilityBigraph drawableGraph(&window);*/

    BicliquePartitioner partitioner = BicliquePartitioner();

    auto p = partitioner.partition(graph, true);
    std::cout << "p = " << p.size() << std::endl;
    for (const auto& G: p) {
        std::cout << *G << std::endl;
    }
    double l = 100;
    std::cout << "a5" << std::endl;

    drawableGraph.resize(size, size);
    drawableGraph.linkGraph(graph);
    drawableGraph.addLine(l);

    window.show();

    return app.exec();
}
