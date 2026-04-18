#ifndef MEMOIRE_DRAWABLEGRAPH_HPP
#define MEMOIRE_DRAWABLEGRAPH_HPP

#include <QPainter>
#include <QWidget>
#include <QPointF>
#include <vector>
#include "../model/Graph.hpp"

class DrawableGraph : public QWidget {
    Q_OBJECT

protected:
    std::shared_ptr<Graph> G;
    std::vector<std::pair<QPointF, QColor>> vertices;

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