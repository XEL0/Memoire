#include <iostream>
#include <QApplication>

#include "model/Graph.hpp"
#include "view/DrawableGraph.hpp"


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    unsigned width = 900;
    unsigned height = 900;

    QWidget window;
    window.setWindowTitle("Visualiser");
    window.resize(width, height);

    QPalette palette = window.palette();
    palette.setColor(QPalette::Window, QColor(227, 221, 206));
    window.setAutoFillBackground(true);
    window.setPalette(palette);

    auto graph = std::make_shared<ComparabilityGraph>(10, 2, 6, 4, height);
    DrawableComparabilityGraph drawableGraph(&window);
    drawableGraph.linkGraph(graph);
    drawableGraph.resize(width, height);

    window.show();

    return app.exec();
}
