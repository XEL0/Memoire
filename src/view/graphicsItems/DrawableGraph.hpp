#ifndef MEMOIRE_DRAWABLEGRAPH_HPP
#define MEMOIRE_DRAWABLEGRAPH_HPP

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>
#include <vector>
#include "../../model/Graph.hpp"

inline auto GREEN = QColor(5, 203, 123);
inline auto PURPLE = QColor(135, 82, 214);
inline auto CYAN = QColor(62, 130, 182);
inline auto RED2 = QColor(218, 54, 12);

struct DrawableVertex {
    QPointF position;
    QColor color;
};


class DrawableGraph : public QGraphicsItem {
protected:
    std::shared_ptr<Graph> G;
    std::unordered_map<unsigned, DrawableVertex> vertices;
    float padding_ratio = 0.1f;
    std::pair<unsigned, unsigned> point_space_bound;
    double scene_width = 1000.0;
    double scene_height = 1000.0;

    [[nodiscard]] virtual QPointF remap(QPointF coordinate) const;
    virtual void embed();
    [[nodiscard]] virtual int minSize() const;
    [[nodiscard]] virtual QColor getColor(const VertexPointer& v) const;
    virtual void drawEdges(QPainter* painter) const;
    virtual void drawVertices(QPainter* painter) const;

    virtual void backgroundPaint(QPainter* painter);
    virtual void foregroundPaint(QPainter* painter);

public:
    explicit DrawableGraph(QGraphicsItem* parent = nullptr);
    ~DrawableGraph() override = default;

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    virtual void linkGraph(const std::shared_ptr<Graph>& G);

    void setSceneDimensions(double width, double height);
};


class DrawableBigraph : public DrawableGraph {
protected:
    [[nodiscard]] QColor getColor(const VertexPointer& v) const override;

public:
    using DrawableGraph::DrawableGraph;
    ~DrawableBigraph() override = default;
};


class DrawableComparabilityGraph : public DrawableGraph {
protected:
    std::pair<double, unsigned> line = {0, 0};

    void embed() override;
    [[nodiscard]] virtual bool canCompareFrom(const VertexPointer& v) const;
    virtual void drawComparisons(QPainter* painter) const;
    void backgroundPaint(QPainter* painter) override;
    void foregroundPaint(QPainter* painter) override;

public:
    using DrawableGraph::DrawableGraph;
    ~DrawableComparabilityGraph() override = default;

    virtual void addLine(double h, unsigned dim) { line = {h, dim}; }
};


class DrawableComparabilityBigraph : public DrawableComparabilityGraph {
protected:
    [[nodiscard]] QColor getColor(const VertexPointer& v) const override;
    [[nodiscard]] bool canCompareFrom(const VertexPointer& v) const override;

public:
    using DrawableComparabilityGraph::DrawableComparabilityGraph;
    ~DrawableComparabilityBigraph() override = default;
};

#endif //MEMOIRE_DRAWABLEGRAPH_HPP