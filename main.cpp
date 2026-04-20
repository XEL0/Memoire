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

    /*auto graph = std::make_shared<Graph>();
    graph->generate(4);
    DrawableGraph drawableGraph(&window);*/

    /*auto graph = std::make_shared<BipartiteGraph>();
    graph->generate(4, 2);
    DrawableBipartiteGraph drawableGraph(&window);*/

    /*auto graph = std::make_shared<ComparabilityGraph>();
    graph->generate(5, 2, 900);
    DrawableComparabilityGraph drawableGraph(&window);*/

    auto graph = std::make_shared<ComparabilityBigraph>();
    graph->generate(5, 5, 2, 900);
    DrawableComparabilityBigraph drawableGraph(&window);

    BicliquePartitioner partitioner = BicliquePartitioner();
    auto l = partitioner.partition(graph);
    std::cout << l << std::endl;
    //double l = 100;*/

    drawableGraph.resize(size, size);
    drawableGraph.linkGraph(graph);
    drawableGraph.addLine(l);

    window.show();

    return app.exec();
}
