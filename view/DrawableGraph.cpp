#include "DrawableGraph.hpp"

#include <ostream>
#include <QPainter>
#include <QtCore/qcoreapplication.h>

DrawableGraph::DrawableGraph(QWidget *parent): QWidget(parent) {}

void DrawableGraph::linkGraph(const std::shared_ptr<Graph>& G) {
    this->G = G;
    this->embed();
}

void DrawableGraph::embed() {
    RandomGenerator rand(0, 900);
    for (const auto v : G->vertices()) {
        this->vertices.push_back(
            {v, QPointF(rand(), rand()), QColor(5, 203, 123)}
            );
    }
}

void DrawableGraph::paintEvent(QPaintEvent*) {
    auto painter = initializePainter();
    this->drawEdges(painter);
    this->drawVertices(painter);
}

std::unique_ptr<QPainter> DrawableGraph::initializePainter() {
    auto painter = std::make_unique<QPainter>(this);
    this->resetPainter(painter);
    return painter;
}

void DrawableGraph::resetPainter(const std::unique_ptr<QPainter> &painter) const {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(Qt::black, 2));
}

void DrawableGraph::drawEdges(const std::unique_ptr<QPainter>& painter) const {
    for (const auto& [u, v] : G->edges()) {
        painter->drawLine(this->vertices[u].position, this->vertices[v].position);
    }
}

void DrawableGraph::drawVertices(const std::unique_ptr<QPainter>& painter) const {
    float radius = 15.f;
    for (const auto& [v, p, color] : this->vertices) {
        painter->setBrush(color);
        painter->drawEllipse(p, radius, radius);
        painter->drawText(p + QPointF(-4, +4), QString::number(v));
    }
    this->resetPainter(painter);
}

void DrawableComparabilityGraph::embed() {
    const auto G2 = static_pointer_cast<ComparabilityGraph>(G);

    for (auto& [v, pos] : G2->embedding()) {
        this->vertices.push_back(
            {v,
                QPointF(pos[0], 900 - pos[1]),
                G2->isInV1(v) ? QColor(5, 203, 123) : QColor(135, 82, 214)}
            );
    }
}

void DrawableComparabilityGraph::drawComparisons(const std::unique_ptr<QPainter>& painter) const {
    painter->setPen(QPen(Qt::black, 2, Qt::DashLine, Qt::RoundCap));

    for (const auto& [v, p, color] : this->vertices) {
        if (G->isBipartite() and G->isInV1(v)) continue;
        painter->drawLine(p + QPointF(-100, 0), p);
        painter->drawLine(p + QPointF(0, 100), p);
    }
    this->resetPainter(painter);
}

void DrawableComparabilityGraph::paintEvent(QPaintEvent *event) {
    auto painter = initializePainter();
    this->drawComparisons(painter);
    this->drawEdges(painter);
    this->drawVertices(painter);
}
