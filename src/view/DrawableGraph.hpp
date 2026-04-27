#ifndef MEMOIRE_DRAWABLEGRAPH_HPP
#define MEMOIRE_DRAWABLEGRAPH_HPP

#include <QPainter>
#include <QWidget>
#include <vector>
#include "../model/Graph.hpp"

inline auto GREEN = QColor(5, 203, 123);
inline auto PURPLE = QColor(135, 82, 214);
inline auto CYAN = QColor(62, 130, 182);
inline auto RED2 = QColor(218, 54, 12);

struct DrawableVertex {
    QPointF position;
    QColor color;
};

class DrawableComparabilityBigraph : public QWidget {
    Q_OBJECT

protected:
    float padding_ratio = 0.05f;
    std::shared_ptr<ComparabilityBigraph> G;
    std::unordered_map<unsigned, DrawableVertex> vertices;
    double line = 0;
    std::pair<unsigned, unsigned> point_space_bound;

    [[nodiscard]] QPointF normalize(QPointF coordinate) const;
    virtual void embed();
    int minSize() const;
    virtual std::unique_ptr<QPainter> initializePainter();
    void resetPainter(const std::unique_ptr<QPainter>& painter) const;
    virtual void drawEdges(const std::unique_ptr<QPainter>& painter) const;
    virtual void drawVertices(const std::unique_ptr<QPainter>& painter) const;
    virtual void drawComparisons(const std::unique_ptr<QPainter>& painter) const;

    virtual void backgroundPaint(const std::unique_ptr<QPainter>& painter);
    virtual void foregroundPaint(const std::unique_ptr<QPainter>& painter);
    void paintEvent(QPaintEvent* event) override;

public:
    explicit DrawableComparabilityBigraph(QWidget *parent = nullptr);

    virtual void linkGraph(const std::shared_ptr<ComparabilityBigraph>& G);
    virtual void addLine(double h){ line = h;};
};


#endif //MEMOIRE_DRAWABLEGRAPH_HPP