#include "../widgets/DrawableGraph.hpp"
#include "../../model/RandomGenerator.hpp"

#include <ostream>
#include <QPainter>
#include <QtCore/qcoreapplication.h>

DrawableGraph::DrawableGraph(QWidget *parent): QWidget(parent) {}
/*DrawableComparabilityBigraph::DrawableComparabilityBigraph(QWidget *parent)
    : QWidget(parent), DrawableGraph(parent), DrawableBigraph(parent), DrawableComparabilityGraph(parent) {}*/

void DrawableGraph::linkGraph(const std::shared_ptr<Graph>& G) {
    this->G = G;
    this->embed();
}

void DrawableGraph::paintEvent(QPaintEvent*) {
    const auto painter = initializePainter();
    this->backgroundPaint(painter);
    this->drawEdges(painter);
    this->drawVertices(painter);
    this->foregroundPaint(painter);
}

std::unique_ptr<QPainter> DrawableGraph::initializePainter() {
    auto painter = std::make_unique<QPainter>(this);
    this->resetPainter(painter);
    return painter;
}

void DrawableGraph::resetPainter(const std::unique_ptr<QPainter> &painter) const {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(Qt::black, minSize() * 1./900));
}

int DrawableGraph::minSize() const {
    return std::min(this->width(), this->height());
}

void DrawableGraph::drawEdges(const std::unique_ptr<QPainter>& painter) const {
    //auto edges = G->makeComplete();
    for (const auto& [u, v] : G->enumerate_edges()) {
        const auto [position_u, color_u] = this->vertices.at(u->getId());
        const auto [position_v, color_v] = this->vertices.at(v->getId());
        painter->drawLine(remap(position_u), remap(position_v));
    }
}

void DrawableGraph::drawVertices(const std::unique_ptr<QPainter>& painter) const {
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

    this->resetPainter(painter);
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

void DrawableComparabilityGraph::embed() {
    auto graph = std::dynamic_pointer_cast<ComparabilityGraph>(G);
    this->point_space_bound = {0, graph->getPointSpaceLimit()};

    for (auto& v : graph->enumerate()) {
        unsigned x_, y_;
        if (graph->getDimension() == 0) {
            x_ = this->width() / 2;
            y_ = this->height() / 2;
        }
        if (graph->getDimension() == 1) {
            x_ = graph->getEmbeddingAt(v, 0);
            y_ = this->height() / 2;
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

QColor DrawableGraph::getColor(const VertexPointer& v) const {
    return CYAN;
}

QColor DrawableBigraph::getColor(const VertexPointer& v) const {
    auto graph = std::dynamic_pointer_cast<Bigraph>(G);
    return graph->isInV1(v) ? CYAN : RED2;
}

QColor DrawableComparabilityBigraph::getColor(const VertexPointer& v) const {
    auto graph = std::dynamic_pointer_cast<ComparabilityBigraph>(G);
    return graph->isInV1(v) ? CYAN : RED2;
}


QPointF DrawableGraph::remap(QPointF coordinate) const {
    const double size = this->point_space_bound.second;
    const double m = minSize();
    const double pad = padding_ratio * m;

    const double x_shift = (this->width() - m) / 2;
    const double y_shift = (this->height() - m) / 2;

    return {pad + (coordinate.x() / size) * (m - 2.0 * pad) + x_shift,
               pad + (1-coordinate.y() / size) * (m - 2.0 * pad) + y_shift};
}

void DrawableComparabilityGraph::drawComparisons(const std::unique_ptr<QPainter>& painter) const {
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
    this->resetPainter(painter);
}

bool DrawableComparabilityGraph::canCompareFrom(const VertexPointer& v) const {
    return true;
}

bool DrawableComparabilityBigraph::canCompareFrom(const VertexPointer& v) const {
    auto graph = std::dynamic_pointer_cast<ComparabilityBigraph>(G);
    return graph->isInV2(v);
}

void DrawableGraph::backgroundPaint(const std::unique_ptr<QPainter>& painter){}

void DrawableComparabilityGraph::backgroundPaint(const std::unique_ptr<QPainter>& painter) {
    this->drawComparisons(painter);
}

void DrawableGraph::foregroundPaint(const std::unique_ptr<QPainter>& painter){}

void DrawableComparabilityGraph::foregroundPaint(const std::unique_ptr<QPainter>& painter) {
    if (line.second == 0) return;
    auto [w, h] = remap(QPointF(0, this->height() - line.first));
    painter->drawLine(QPointF(0, h), QPointF(this->width(), h));
}