#include <iostream>
#include <QApplication>
#include <QJsonObject>

#include "model/Algorithms.hpp"
#include "model/Graph.hpp"
#include "view/graphicsItems/DrawableGraph.hpp"
#include "view/windows/MainWindow.hpp"
#include "model/PartitionTreeLoader.hpp"


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    //auto json = PartitionTreeLoader::loadPartitionTreeFromJson("partition_tree.json");
    //PartitionTreeLoader::printPartitionTree(json);

    MainWindow window;
    window.show();

    return app.exec();
}
/*
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

    std::vector<VertexPointer> V{
        std::make_shared<ColoredEmbeddedVertex>(1, 0, std::vector<unsigned>{100, 200}),
        std::make_shared<ColoredEmbeddedVertex>(2, 0, std::vector<unsigned>{200, 400}),
        std::make_shared<ColoredEmbeddedVertex>(3, 0, std::vector<unsigned>{300, 300}),
        std::make_shared<ColoredEmbeddedVertex>(0, 1, std::vector<unsigned>{0, 0}),
        std::make_shared<ColoredEmbeddedVertex>(4, 1, std::vector<unsigned>{400, 500}),
        std::make_shared<ColoredEmbeddedVertex>(5, 1, std::vector<unsigned>{500, 100}),
        std::make_shared<ColoredEmbeddedVertex>(6, 1, std::vector<unsigned>{600, 600}),
        std::make_shared<ColoredEmbeddedVertex>(7, 1, std::vector<unsigned>{700, 700})
    };
    auto graph = std::make_shared<ComparabilityBigraph>(std::move(V), 3 , 5, 2, 900);


    std::vector<VertexPointer> V{
        std::make_shared<ColoredEmbeddedVertex>(3, 0, std::vector<unsigned>{146, 394}),
        std::make_shared<ColoredEmbeddedVertex>(1, 0, std::vector<unsigned>{457, 841}),
        std::make_shared<ColoredEmbeddedVertex>(6, 0, std::vector<unsigned>{379, 17}),
        std::make_shared<ColoredEmbeddedVertex>(5, 0, std::vector<unsigned>{95, 467}),
        std::make_shared<ColoredEmbeddedVertex>(2, 0, std::vector<unsigned>{197, 530}),
        std::make_shared<ColoredEmbeddedVertex>(7, 1, std::vector<unsigned>{3, 756}),
        std::make_shared<ColoredEmbeddedVertex>(8, 1, std::vector<unsigned>{426, 705}),
        std::make_shared<ColoredEmbeddedVertex>(0, 1, std::vector<unsigned>{896, 316}),
        std::make_shared<ColoredEmbeddedVertex>(4, 1, std::vector<unsigned>{196, 336}),
        std::make_shared<ColoredEmbeddedVertex>(9, 1, std::vector<unsigned>{831, 118}),
    };
    auto graph = std::make_shared<ComparabilityBigraph>(std::move(V), 5 , 5, 2, 900);

    auto graph = std::make_shared<ComparabilityBigraph>();
    graph->generate(5, 5, 2, 900);
    std::cout << *graph << std::endl;
    graph->makeComplete();

    DrawableComparabilityBigraph drawableGraph(&window);

    auto graph = std::make_shared<ComparabilityBigraph>();
    graph->generate(5, 5, 2, 900);
    DrawableComparabilityBigraph drawableGraph(&window);

    BicliquePartitioner partitioner = BicliquePartitioner();
    auto p = partitioner.partition(graph, false);
    std::cout << "p = " << p.size() << std::endl;
    for (const auto& G: p) {
        std::cout << *G << std::endl;
    }
    double l = 100;

    drawableGraph.resize(size, size);
    drawableGraph.linkGraph(graph);
    //drawableGraph.addLine(l);

    window.show();

    return app.exec();
}*/
