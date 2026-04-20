#include "DrawableGraph.hpp"
#include "../model/RandomGenerator.hpp"

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
        this->vertices[v] = {
            QPointF(rand(), rand()),
            CYAN
        };
    }
}

void DrawableBipartiteGraph::embed() {
    const auto G2 = std::dynamic_pointer_cast<BipartiteGraph>(G);
    const unsigned h = this->height();
    const unsigned pad = static_cast<unsigned>(padding_ratio) * h;

    RandomGenerator rand(pad , h - pad);
    for (const auto v : G->vertices()) {
        this->vertices[v] = {
            QPointF(rand(), rand()),
            G2->isInV1(v) ? CYAN : RED
        };
    }
}

void DrawableGraph::backgroundPaint(const std::unique_ptr<QPainter>&) {}
void DrawableGraph::foregroundPaint(const std::unique_ptr<QPainter>&) {}
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
    painter->setPen(QPen(Qt::black, this->height() * 1./900));
}

void DrawableGraph::drawEdges(const std::unique_ptr<QPainter>& painter) const {
    for (const auto& [u, v] : G->edges()) {
        const auto [position_u, color_u] = this->vertices.at(u);
        const auto [position_v, color_v] = this->vertices.at(v);
        painter->drawLine(position_u, position_v);
    }
}

void DrawableGraph::drawVertices(const std::unique_ptr<QPainter>& painter) const {
    const float radius = static_cast<float>(this->height()) * 0.02f;
    const float writing_size = static_cast<float>(this->height()) * 1.5f/900;

    for (const auto& [v, dv] : this->vertices) {
        const auto& p = dv.position;
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



void DrawableComparabilityGraph::embed() {
    const auto G2 = std::dynamic_pointer_cast<ComparabilityGraph>(G);
    this->point_space_bound = {0, G2->getPointSpaceLimit()};

    for (auto& [v, pos] : G2->embedding()) {
        auto [x, y] = normalize({static_cast<qreal>(pos[0]), static_cast<qreal>(pos[1])});
        this->vertices[v] = {
            QPointF(x, this->height() - y),
            CYAN
        };
    }
}

void DrawableComparabilityBigraph::embed() {
    const auto G2 = std::dynamic_pointer_cast<ComparabilityBigraph>(G);
    this->point_space_bound = {0, G2->getPointSpaceLimit()};

    for (auto& [v, pos] : G2->embedding()) {
        auto [x, y] = normalize({static_cast<qreal>(pos[0]), static_cast<qreal>(pos[1])});
        this->vertices[v] = {
            QPointF(x, this->height() - y),
            G2->isInV1(v) ? CYAN : RED
        };
    }
}


QPointF DrawableComparabilityGraph::normalize(QPointF coordinate) const {
    const double size = this->point_space_bound.second;
    const double h = this->height();
    const double pad = padding_ratio * h;

    return {pad + (coordinate.x() / size) * (h - 2.0 * pad),
               pad + (coordinate.y() / size) * (h - 2.0 * pad)};
}

void DrawableComparabilityGraph::drawComparisons(const std::unique_ptr<QPainter>& painter) const {
    const float length = static_cast<float>(this->height()) * 0.1f;
    painter->setPen(QPen(Qt::darkGray, 1, Qt::DashLine, Qt::RoundCap));

    for (const auto& [v, dv] : this->vertices) {
        const auto& p = dv.position;
        painter->drawLine(p + QPointF(-length, 0), p);
        painter->drawLine(p + QPointF(0, length), p);
    }
    this->resetPainter(painter);
}

void DrawableComparabilityBigraph::drawComparisons(const std::unique_ptr<QPainter>& painter) const {
    const auto G2 = std::dynamic_pointer_cast<ComparabilityBigraph>(G);
    const float length = static_cast<float>(this->height()) * 0.1f;
    painter->setPen(QPen(Qt::darkGray, 1, Qt::DashLine, Qt::RoundCap));

    for (const auto& [v, dv] : this->vertices) {
        const auto& p = dv.position;
        if (G2->isInV1(v)) continue;
        painter->drawLine(p + QPointF(-length, 0), p);
        painter->drawLine(p + QPointF(0, length), p);
    }
    this->resetPainter(painter);
}

void DrawableComparabilityGraph::backgroundPaint(const std::unique_ptr<QPainter>& painter) {
    this->drawComparisons(painter);
}

void DrawableComparabilityGraph::foregroundPaint(const std::unique_ptr<QPainter>& painter) {
    auto [w, h] = normalize(QPointF(0, this->height() - line));
    painter->drawLine(QPointF(0, h), QPointF(this->width(), h));
}
