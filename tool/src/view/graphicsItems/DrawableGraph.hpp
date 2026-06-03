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
    QColor backgroundColor = Qt::white;

    [[nodiscard]] virtual QPointF remap(QPointF coordinate) const;
    virtual void embed();
    [[nodiscard]] virtual int minSize() const;
    [[nodiscard]] virtual QColor getColor(const VertexPointer& v) const;
    virtual void drawEdges(QPainter* painter) const;
    virtual void drawVertices(QPainter* painter, const std::unordered_map<unsigned, DrawableVertex>& V) const;
    virtual std::pair<float, float> findRadiusAndWritingSize() const;
    virtual void backgroundPaint(QPainter* painter);
    virtual void foregroundPaint(QPainter* painter);

public:
    explicit DrawableGraph(QGraphicsItem* parent = nullptr);
    ~DrawableGraph() override = default;

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    virtual void linkGraph(const std::shared_ptr<Graph>& G);

    void setSceneDimensions(double width, double height);
    void setBackgroundColor(const QColor& color) { backgroundColor = color; }
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
    float radiusIn0D = 0;
    float writingIn0D = 0;

    std::pair<float, float> findRadiusAndWritingSize() const override;
    void embed() override;
    virtual void embedIn0D();
    [[nodiscard]] virtual bool canCompareFrom(const VertexPointer& v) const;
    virtual void drawComparisons(QPainter* painter) const;
    void drawAxes(QPainter* painter) const;
    void backgroundPaint(QPainter* painter) override;
    void foregroundPaint(QPainter* painter) override;
    void drawEdges(QPainter* painter) const override;

public:
    using DrawableGraph::DrawableGraph;
    ~DrawableComparabilityGraph() override = default;

    virtual void addLine(double h, unsigned dim) { line = {h, dim}; }
};


class DrawableComparabilityBigraph : public DrawableComparabilityGraph {
protected:
    [[nodiscard]] std::pair<float, float> findRadiusAndWritingSize() const override;
    [[nodiscard]] QColor getColor(const VertexPointer& v) const override;
    [[nodiscard]] bool canCompareFrom(const VertexPointer& v) const override;
    void embedIn0D() override;

public:
    using DrawableComparabilityGraph::DrawableComparabilityGraph;
    ~DrawableComparabilityBigraph() override = default;
};


class DrawableTerrainVisibilityGraph : public DrawableGraph {
protected:
    bool draw_in_terrain_order = false;
    void embed() override;
    void drawVertices(QPainter* painter, const std::unordered_map<unsigned, DrawableVertex>& V) const override;
    void drawEdges(QPainter* painter) const override;
    [[nodiscard]] bool isTerrainEdge(const unsigned id1, const unsigned id2) const { return (id1 + 1 == id2) or (id2 + 1 == id1);}
    std::shared_ptr<Terrain> terrain;
    std::unordered_map<unsigned, DrawableVertex> terrain_ordered_vertices;

public:
    using DrawableGraph::DrawableGraph;
    ~DrawableTerrainVisibilityGraph() override = default;
    void linkGraphs(const std::shared_ptr<Graph>& G, const std::shared_ptr<Terrain>& terrain);
    void drawAsTerrain(const bool mode) { draw_in_terrain_order = mode; }
    [[nodiscard]] bool isDrawAsTerrain() const { return draw_in_terrain_order; }
};

#endif //MEMOIRE_DRAWABLEGRAPH_HPP