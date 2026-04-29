#ifndef MEMOIRE_DRAWABLEGRAPH_HPP
#define MEMOIRE_DRAWABLEGRAPH_HPP

#include <QPainter>
#include <QWidget>
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




class DrawableGraph : public QWidget {
    Q_OBJECT
protected:
    std::shared_ptr<Graph> G;
    std::unordered_map<unsigned, DrawableVertex> vertices;
    float padding_ratio = 0.1f;
    std::pair<unsigned, unsigned> point_space_bound;

    [[nodiscard]] virtual QPointF normalize(QPointF coordinate) const;
    virtual void embed();
    [[nodiscard]] virtual int minSize() const;
    virtual std::unique_ptr<QPainter> initializePainter();
    [[nodiscard]] virtual QColor getColor(const VertexPointer& v) const;
    virtual void resetPainter(const std::unique_ptr<QPainter>& painter) const;
    virtual void drawEdges(const std::unique_ptr<QPainter>& painter) const;
    virtual void drawVertices(const std::unique_ptr<QPainter>& painter) const;

    virtual void backgroundPaint(const std::unique_ptr<QPainter>& painter);
    virtual void foregroundPaint(const std::unique_ptr<QPainter>& painter);
    void paintEvent(QPaintEvent* event) override;

public:
    explicit DrawableGraph(QWidget *parent = nullptr);
    ~DrawableGraph() override = default;
    virtual void linkGraph(const std::shared_ptr<Graph>& G);
};




class DrawableBigraph : public DrawableGraph {
    Q_OBJECT
protected:
    [[nodiscard]] QColor getColor(const VertexPointer& v) const override;
public:
    using DrawableGraph::DrawableGraph;
    ~DrawableBigraph() override = default;
};




class DrawableComparabilityGraph : public DrawableGraph {
    Q_OBJECT
protected:
    std::pair<double, unsigned> line = {0, 0};
    void embed() override;

    [[nodiscard]] virtual bool canCompareFrom(const VertexPointer &v) const;
    virtual void drawComparisons(const std::unique_ptr<QPainter>& painter) const;
    void backgroundPaint(const std::unique_ptr<QPainter>& painter) override;
    void foregroundPaint(const std::unique_ptr<QPainter>& painter) override;
public:
    using DrawableGraph::DrawableGraph;
    ~DrawableComparabilityGraph() override = default;
    virtual void addLine(double h, unsigned dim){ line = {h, dim};};
};




class DrawableComparabilityBigraph : public DrawableComparabilityGraph {
    Q_OBJECT

protected:
    //virtual void drawEdges(const std::unique_ptr<QPainter>& painter) const;
    [[nodiscard]] QColor getColor(const VertexPointer& v) const override;
    [[nodiscard]] bool canCompareFrom(const VertexPointer &v) const override;

public:
    using DrawableComparabilityGraph::DrawableComparabilityGraph;
    //explicit DrawableComparabilityBigraph(QWidget *parent = nullptr);
    ~DrawableComparabilityBigraph() override = default;
};


#endif //MEMOIRE_DRAWABLEGRAPH_HPP