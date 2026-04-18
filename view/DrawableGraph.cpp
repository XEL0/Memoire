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
    const unsigned h = this->height();
    const unsigned pad = static_cast<unsigned>(padding_ratio) * h;

    RandomGenerator rand(pad , h - pad);
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
    painter->setPen(QPen(Qt::black, this->height() * 1./900));
}

void DrawableGraph::drawEdges(const std::unique_ptr<QPainter>& painter) const {
    for (const auto& [u, v] : G->edges()) {
        painter->drawLine(this->vertices[u].position, this->vertices[v].position);
    }
}

void DrawableGraph::drawVertices(const std::unique_ptr<QPainter>& painter) const {
    float radius = static_cast<float>(this->height()) * 0.02f;
    float writing_size = static_cast<float>(this->height()) * 1.5f/900;
    for (const auto& [v, p, color] : this->vertices) {
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

void DrawableComparabilityGraph::embed() {
    const auto G2 = static_pointer_cast<ComparabilityGraph>(G);
    const double size = G2->getUB();
    const double h = this->height();
    const double pad = padding_ratio * h;

    for (auto& [v, pos] : G2->embedding()) {
        const double x = pad + (pos[0] / size) * (h - 2.0 * pad);
        const double y = pad + (pos[1] / size) * (h - 2.0 * pad);

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
