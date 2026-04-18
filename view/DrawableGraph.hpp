#ifndef MEMOIRE_DRAWABLEGRAPH_HPP
#define MEMOIRE_DRAWABLEGRAPH_HPP

#include <QPainter>
#include <QWidget>
#include <vector>
#include "../model/Graph.hpp"

inline auto GREEN = QColor(5, 203, 123);
inline auto PURPLE = QColor(135, 82, 214);
inline auto CYAN = QColor(62, 130, 182);
inline auto RED = QColor(218, 54, 12);

struct DrawableVertex {
    QPointF position;
    QColor color;
};

class DrawableGraph : public QWidget {
    Q_OBJECT

protected:
    float padding_ratio = 0.05f;
    std::shared_ptr<Graph> G;
    std::unordered_map<Vertex, DrawableVertex> vertices;
    double line = 0;

    virtual void embed();
    virtual std::unique_ptr<QPainter> initializePainter();
    void resetPainter(const std::unique_ptr<QPainter>& painter) const;
    virtual void drawEdges(const std::unique_ptr<QPainter>& painter) const;
    virtual void drawVertices(const std::unique_ptr<QPainter>& painter) const;

    virtual void backgroundPaint(const std::unique_ptr<QPainter>& painter);
    virtual void foregroundPaint(const std::unique_ptr<QPainter>& painter);
    void paintEvent(QPaintEvent* event) override;

public:
    explicit DrawableGraph(QWidget *parent = nullptr);

    virtual void linkGraph(const std::shared_ptr<Graph>& G);
    virtual void addLine(double h){ line = h;};
};

class DrawableComparabilityGraph : public DrawableGraph {
protected:
    std::pair<unsigned, unsigned> point_space_bound;

    [[nodiscard]] QPointF normalize(QPointF coordinate) const;
    void embed() override;
    void drawComparisons(const std::unique_ptr<QPainter>& painter) const;
    void backgroundPaint(const std::unique_ptr<QPainter>& painter) override;
    void foregroundPaint(const std::unique_ptr<QPainter>& painter) override;
public:
    using DrawableGraph::DrawableGraph;
};

#endif //MEMOIRE_DRAWABLEGRAPH_HPP