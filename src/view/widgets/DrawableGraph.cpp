#include "../widgets/DrawableGraph.hpp"
#include "../../model/RandomGenerator.hpp"

#include <ostream>
#include <QPainter>
#include <QtCore/qcoreapplication.h>

DrawableGraph::DrawableGraph(QGraphicsItem* parent)
    : QGraphicsItem(parent) {
    setAcceptHoverEvents(true);
}

QRectF DrawableGraph::boundingRect() const {
    return QRectF(0, 0, scene_width, scene_height);
}

void DrawableGraph::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(Qt::black, minSize() * 1./900));

    this->backgroundPaint(painter);
    this->drawEdges(painter);
    this->drawVertices(painter);
    this->foregroundPaint(painter);
}

void DrawableGraph::setSceneDimensions(double width, double height) {
    prepareGeometryChange();
    scene_width = width;
    scene_height = height;
    update();
}

int DrawableGraph::minSize() const {
    return static_cast<int>(std::min(scene_width, scene_height));
}

void DrawableGraph::linkGraph(const std::shared_ptr<Graph>& G) {
    this->G = G;
    this->embed();
    update();
}

void DrawableGraph::drawEdges(QPainter* painter) const {
    for (const auto& [u, v] : G->enumerate_edges()) {
        const auto [position_u, color_u] = this->vertices.at(u->getId());
        const auto [position_v, color_v] = this->vertices.at(v->getId());
        painter->drawLine(remap(position_u), remap(position_v));
    }
}

void DrawableGraph::drawVertices(QPainter* painter) const {
    const float radius = static_cast<float>(minSize()) * 0.02f;
    const float writing_size = static_cast<float>(minSize()) * 1.5f/900;

    for (const auto& [v, dv] : this->vertices) {
        const auto& p = remap(dv.position);
        const auto& color = dv.color;

        painter->setPen(Qt::NoPen);
        painter->setBrush(color);
        painter->drawEllipse(p, radius, radius);

        painter->setPen(QPen(Qt::white, writing_size));

        QRectF textRect(p.x() - radius, p.y() - radius, 2 * radius, 2 * radius);
        QFont font = painter->font();
        font.setPointSizeF(radius * 0.8);
        painter->setFont(font);
        painter->drawText(textRect, Qt::AlignCenter, QString::number(v));
    }
}

void DrawableGraph::embed() {
    auto rg = RandomGenerator(0, 1000);
    this->point_space_bound = {0, 1000};

    for (auto& v : G->enumerate()) {
        this->vertices[v->getId()] = {
            QPointF(rg(), rg()),
            getColor(v)
        };
    }
}

QColor DrawableGraph::getColor(const VertexPointer& v) const {
    return CYAN;
}

QPointF DrawableGraph::remap(QPointF coordinate) const {
    const double size = this->point_space_bound.second;
    const double m = minSize();
    const double pad = padding_ratio * m;

    const double x_shift = (scene_width - m) / 2;
    const double y_shift = (scene_height - m) / 2;

    return {pad + (coordinate.x() / size) * (m - 2.0 * pad) + x_shift,
            pad + (1 - coordinate.y() / size) * (m - 2.0 * pad) + y_shift};
}

void DrawableGraph::backgroundPaint(QPainter* painter) { Q_UNUSED(painter);}

void DrawableGraph::foregroundPaint(QPainter* painter) {Q_UNUSED(painter);}

QColor DrawableBigraph::getColor(const VertexPointer& v) const {
    auto graph = std::dynamic_pointer_cast<Bigraph>(G);
    return graph->isInV1(v) ? CYAN : RED2;
}

void DrawableComparabilityGraph::embed() {
    auto graph = std::dynamic_pointer_cast<ComparabilityGraph>(G);
    this->point_space_bound = {0, graph->getPointSpaceLimit()};

    for (auto& v : graph->enumerate()) {
        unsigned x_, y_;
        if (graph->getDimension() == 0) {
            x_ = static_cast<unsigned>(scene_width / 2);
            y_ = static_cast<unsigned>(scene_height / 2);
        }
        else if (graph->getDimension() == 1) {
            x_ = graph->getEmbeddingAt(v, 0);
            y_ = static_cast<unsigned>(scene_height / 2);
        } else {
            x_ = graph->getEmbeddingAt(v, 0);
            y_ = graph->getEmbeddingAt(v, 1);
        }
        this->vertices[v->getId()] = {
            QPointF(x_, y_),
            getColor(v)
        };
    }
}

void DrawableComparabilityGraph::drawComparisons(QPainter* painter) const {
    auto graph = std::dynamic_pointer_cast<ComparabilityGraph>(G);
    const float length = static_cast<float>(minSize()) * 0.1f;
    painter->setPen(QPen(Qt::darkGray, 1, Qt::DashLine, Qt::RoundCap));

    for (const auto& v : graph->enumerate()) {
        const auto& dv = this->vertices.at(v->getId());
        const auto& p = remap(dv.position);
        if (not canCompareFrom(v)) continue;
        painter->drawLine(p + QPointF(-length, 0), p);
        painter->drawLine(p + QPointF(0, length), p);
    }
}

bool DrawableComparabilityGraph::canCompareFrom(const VertexPointer& v) const {
    return true;
}

void DrawableComparabilityGraph::backgroundPaint(QPainter* painter) {
    this->drawComparisons(painter);
}

void DrawableComparabilityGraph::foregroundPaint(QPainter* painter) {
    if (line.second == 0) return;
    auto h = remap(QPointF(0, line.first)).y();
    painter->drawLine(QPointF(0, h), QPointF(scene_width, h));
}


QColor DrawableComparabilityBigraph::getColor(const VertexPointer& v) const {
    auto graph = std::dynamic_pointer_cast<ComparabilityBigraph>(G);
    return graph->isInV1(v) ? CYAN : RED2;
}

bool DrawableComparabilityBigraph::canCompareFrom(const VertexPointer& v) const {
    auto graph = std::dynamic_pointer_cast<ComparabilityBigraph>(G);
    return graph->isInV2(v);
}