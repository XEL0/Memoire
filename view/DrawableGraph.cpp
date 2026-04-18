#include "DrawableGraph.hpp"

#include <iostream>
#include <ostream>
#include <QPainter>
#include <QtCore/qcoreapplication.h>

DrawableGraph::DrawableGraph(QWidget *parent): QWidget(parent) {}

void DrawableGraph::linkGraph(const std::shared_ptr<Graph>& G) {
    this->G = G;
    this->embed();
}

void DrawableGraph::embed() {
    RandomGenerator rand(0, this->size().width());
    for (const auto v : G->vertices()) {
        this->vertices.push_back(
            {v, QPointF(rand(), rand()), CYAN}
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
    painter->setPen(QPen(Qt::black, 1));
}

void DrawableGraph::drawEdges(const std::unique_ptr<QPainter>& painter) const {
    for (const auto& [u, v] : G->edges()) {
        painter->drawLine(this->vertices[u].position, this->vertices[v].position);
    }
}

void DrawableGraph::drawVertices(const std::unique_ptr<QPainter>& painter) const {
    float radius = static_cast<float>(this->height()) * 0.02f;
    for (const auto& [v, p, color] : this->vertices) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(color);
        painter->drawEllipse(p, radius, radius);

        painter->setPen(QPen(Qt::white, 1.5));

        QRectF textRect(p.x() - radius, p.y() - radius, 2 * radius, 2 * radius);
        painter->drawText(textRect, Qt::AlignCenter, QString::number(v));
    }

    this->resetPainter(painter);
}

void DrawableComparabilityGraph::embed() {
    const auto G2 = static_pointer_cast<ComparabilityGraph>(G);
    const double ww = window()->size().width();
    const double wh = window()->size().height();
    const double w = this->width();
    const double h = this->height();
    const double padX = padding_ratio * w;
    const double padY = padding_ratio * h;

    for (auto& [v, pos] : G2->embedding()) {
        const double x = padX + (pos[0] / ww) * (w - 2.0 * padX);
        const double y = padY + (pos[1] / wh) * (h - 2.0 * padY);

        this->vertices.push_back(
            {v,
             QPointF(x, h - y),
             G2->isInV1(v) ? CYAN : RED}
        );
    }
}

void DrawableComparabilityGraph::drawComparisons(const std::unique_ptr<QPainter>& painter) const {
    float length = static_cast<float>(this->height()) * 0.1f;
    painter->setPen(QPen(Qt::darkGray, 1, Qt::DashLine, Qt::RoundCap));

    for (const auto& [v, p, color] : this->vertices) {
        if (G->isBipartite() and G->isInV1(v)) continue;
        painter->drawLine(p + QPointF(-length, 0), p);
        painter->drawLine(p + QPointF(0, length), p);
    }
    this->resetPainter(painter);
}

void DrawableComparabilityGraph::paintEvent(QPaintEvent *event) {
    auto painter = initializePainter();
    this->drawComparisons(painter);
    this->drawEdges(painter);
    this->drawVertices(painter);
}
