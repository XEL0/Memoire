#include <iostream>
#include <QApplication>

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

    unsigned V = 10;
    //auto graph = std::make_shared<ComparabilityGraph>(V, 2, V * 100);
    auto graph = std::make_shared<ComparabilityGraph>(V, 2, 5, 5, V * 100);
    DrawableComparabilityGraph drawableGraph(&window);
    drawableGraph.resize(size, size);
    drawableGraph.linkGraph(graph);

    window.show();

    return app.exec();
}
