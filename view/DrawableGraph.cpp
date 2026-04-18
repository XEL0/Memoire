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
    RandomGenerator rand(0, 900);
    for (auto v : G->vertices()) {
        this->vertices.emplace_back(QPointF(rand(), rand()),
                                    QColor(5, 203, 123));
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
        painter->drawLine(this->vertices[u].first, this->vertices[v].first);
    }
}

void DrawableGraph::drawVertices(const std::unique_ptr<QPainter>& painter) const {
    float radius = 15.f;
    int i = 0;
    for (const auto& [p, color] : this->vertices) {
        painter->setBrush(color);
        std::cout << p.x() << " " << p.y() << std::endl;
        painter->drawEllipse(p, radius, radius);
        painter->drawText(p + QPointF(-4, +4), QString::number(i));
        i++;
    }
    this->resetPainter(painter);
}

void DrawableComparabilityGraph::embed() {
    const auto G2 = static_pointer_cast<ComparabilityGraph>(G);
    unsigned i = 0;
    for (auto& v : G2->embedding()) {
        std::cout << "(" << v[0] << ", " << v[1] << ")" << std::endl;
        std::cout << G->isInV1(i) << " " << G->isInV2(i) << std::endl;
        this->vertices.emplace_back(QPointF(v[0], 900 - v[1]),
                                    G2->isInV1(i) ? QColor(5, 203, 123) : QColor(135, 82, 214));
        i++;
    }
}

void DrawableComparabilityGraph::drawComparisons(const std::unique_ptr<QPainter>& painter) const {
    painter->setPen(QPen(Qt::black, 2, Qt::DashLine, Qt::RoundCap));

    unsigned i = -1;
    for (const auto& [p, color] : this->vertices) {
        i++;
        std::cout << G->isInV1(i) << " " << G->isInV2(i) << std::endl;
        if (G->isBipartite() and G->isInV1(i)) continue;
        std::cout << "line" << std::endl;
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
