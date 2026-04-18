#ifndef MEMOIRE_DRAWABLEGRAPH_HPP
#define MEMOIRE_DRAWABLEGRAPH_HPP

#include <QPainter>
#include <QWidget>
#include <vector>
#include "../model/Graph.hpp"

inline QColor GREEN = QColor(5, 203, 123);
inline QColor PURPLE = QColor(135, 82, 214);
inline QColor CYAN = QColor(62, 130, 182);
inline QColor RED = QColor(218, 54, 12);

struct DrawableVertex {
    Vertex v{};
    QPointF position;
    QColor color;
};

class DrawableGraph : public QWidget {
    Q_OBJECT

protected:
    float padding_ratio = 0.05f;
    std::shared_ptr<Graph> G;
    std::vector<DrawableVertex> vertices;

    virtual void embed();
    virtual std::unique_ptr<QPainter> initializePainter();
    void resetPainter(const std::unique_ptr<QPainter>& painter) const;
    virtual void drawEdges(const std::unique_ptr<QPainter>& painter) const;
    virtual void drawVertices(const std::unique_ptr<QPainter>& painter) const;
    void paintEvent(QPaintEvent* event) override;

public:
    explicit DrawableGraph(QWidget *parent = nullptr);

    virtual void linkGraph(const std::shared_ptr<Graph>& G);
};

class DrawableComparabilityGraph : public DrawableGraph {
protected:
    void embed() override;
    void drawComparisons(const std::unique_ptr<QPainter>& painter) const;
    void paintEvent(QPaintEvent* event) override;
public:
    using DrawableGraph::DrawableGraph;
};

#endif //MEMOIRE_DRAWABLEGRAPH_HPP