#include "DrawableGraph.hpp"
#include "../model/RandomGenerator.hpp"

#include <iostream>
#include <ostream>
#include <QPainter>
#include <QtCore/qcoreapplication.h>

DrawableComparabilityBigraph::DrawableComparabilityBigraph(QWidget *parent): QWidget(parent) {}

void DrawableComparabilityBigraph::linkGraph(const std::shared_ptr<ComparabilityBigraph>& G) {
    this->G = G;
    this->embed();
}

void DrawableComparabilityBigraph::paintEvent(QPaintEvent*) {
    const auto painter = initializePainter();
    this->backgroundPaint(painter);
    this->drawEdges(painter);
    this->drawVertices(painter);
    this->foregroundPaint(painter);
}

std::unique_ptr<QPainter> DrawableComparabilityBigraph::initializePainter() {
    auto painter = std::make_unique<QPainter>(this);
    this->resetPainter(painter);
    return painter;
}

void DrawableComparabilityBigraph::resetPainter(const std::unique_ptr<QPainter> &painter) const {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(Qt::black, this->height() * 1./900));
}

void DrawableComparabilityBigraph::drawEdges(const std::unique_ptr<QPainter>& painter) const {
    auto edges = G->makeComplete();
    for (const auto& [u, v] : *edges) {
        const auto [position_u, color_u] = this->vertices.at(u->getId());
        const auto [position_v, color_v] = this->vertices.at(v->getId());
        painter->drawLine(position_u, position_v);
    }
}

void DrawableComparabilityBigraph::drawVertices(const std::unique_ptr<QPainter>& painter) const {
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





void DrawableComparabilityBigraph::embed() {
    this->point_space_bound = {0, G->getPointSpaceLimit()};

    for (auto& v : G->enumerate()) {
        auto x_ = G->getEmbeddingAt(v, 0);
        auto y_ = G->getEmbeddingAt(v, 1);
        auto [x, y] = normalize({static_cast<qreal>(x_), static_cast<qreal>(y_)});
        this->vertices[v->getId()] = {
            QPointF(x, this->height() - y),
            G->isInV1(v) ? CYAN : RED2
        };
    }
}


QPointF DrawableComparabilityBigraph::normalize(QPointF coordinate) const {
    const double size = this->point_space_bound.second;
    const double h = this->height();
    const double pad = padding_ratio * h;

    return {pad + (coordinate.x() / size) * (h - 2.0 * pad),
               pad + (coordinate.y() / size) * (h - 2.0 * pad)};
}

void DrawableComparabilityBigraph::drawComparisons(const std::unique_ptr<QPainter>& painter) const {
    const float length = static_cast<float>(this->height()) * 0.1f;
    painter->setPen(QPen(Qt::darkGray, 1, Qt::DashLine, Qt::RoundCap));

    for (const auto& v : G->enumerate()) {
        const auto& dv = this->vertices.at(v->getId());
        const auto& p = dv.position;
        if (G->isInV1(v)) continue;
        painter->drawLine(p + QPointF(-length, 0), p);
        painter->drawLine(p + QPointF(0, length), p);
    }
    this->resetPainter(painter);
}

void DrawableComparabilityBigraph::backgroundPaint(const std::unique_ptr<QPainter>& painter) {
    this->drawComparisons(painter);
}

void DrawableComparabilityBigraph::foregroundPaint(const std::unique_ptr<QPainter>& painter) {
    auto [w, h] = normalize(QPointF(0, this->height() - line));
    painter->drawLine(QPointF(0, h), QPointF(this->width(), h));
}
