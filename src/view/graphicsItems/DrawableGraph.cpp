#include "../graphicsItems/DrawableGraph.hpp"
#include "../../model/RandomGenerator.hpp"

#include <ostream>
#include <QPainter>
#include <QtCore/qcoreapplication.h>

DrawableGraph::DrawableGraph(QGraphicsItem* parent): QGraphicsItem(parent) {
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
    painter->fillRect(QRectF(0, 0, scene_width, scene_height), backgroundColor);
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
    painter->setPen(QPen(Qt::black, 1));
    for (const auto& [u, v] : G->enumerate_edges()) {
        const auto [position_u, color_u] = this->vertices.at(u->getId());
        const auto [position_v, color_v] = this->vertices.at(v->getId());
        painter->drawLine(remap(position_u), remap(position_v));
    }
}

void DrawableGraph::drawVertices(QPainter* painter) const {
    const auto [radius, writing_size] = findRadiusAndWritingSize();

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

std::pair<float, float> DrawableGraph::findRadiusAndWritingSize() const {
    const float radius = static_cast<float>(minSize()) * 0.02f;
    const float writing_size = static_cast<float>(minSize()) * 1.5f/900;
    return {radius, writing_size};
}

std::pair<float, float> DrawableComparabilityGraph::findRadiusAndWritingSize() const {
    return DrawableGraph::findRadiusAndWritingSize();
}

std::pair<float, float> DrawableComparabilityBigraph::findRadiusAndWritingSize() const {
    const auto graph = std::dynamic_pointer_cast<ComparabilityBigraph>(G);
    if (graph->getDimension() != 0) return DrawableGraph::findRadiusAndWritingSize();

    const unsigned p = graph->V1_size();
    const unsigned q = graph->V2_size();
    const double height = point_space_bound.second;

    const double zone_size_p = (p > 0) ? height / static_cast<double>(p) : height;
    const double zone_size_q = (q > 0) ? height / static_cast<double>(q) : height;
    const double max_radius_virtual = std::min(zone_size_p / 4.0, zone_size_q / 4.0);
    const double radius_virtual = std::min(100.0, max_radius_virtual);

    const double m = minSize();
    const double pad = padding_ratio * m;
    const double drawable_size = m - 2.0 * pad;
    const double scale_factor = drawable_size / height;

    float radius = static_cast<float>(radius_virtual * scale_factor);
    float writing_size = radius * 1.5f / (0.02f * 900.0f);

    return {radius, writing_size};
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

QPointF DrawableGraph::remap(const QPointF coordinate) const {
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
    if (graph->getDimension() == 0) {
        embedIn0D();
        return;
    }
    this->point_space_bound = {0, graph->getPointSpaceLimit()};

    for (auto& v : graph->enumerate()) {
        unsigned x_, y_;
        if (graph->getDimension() == 1) {
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
    if (graph->getDimension() <= 1) return;
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
    painter->setPen(QPen(QColor(255, 218, 96), 1));
    if (line.second == 1) {
        auto h = remap(QPointF(line.first, 0)).x();
        painter->drawLine(QPointF(h, 0), QPointF(h, scene_height));
    }
    else if (line.second == 2) {
        auto h = remap(QPointF(0, line.first)).y();
        painter->drawLine(QPointF(0, h), QPointF(scene_width, h));
    }
}


QColor DrawableComparabilityBigraph::getColor(const VertexPointer& v) const {
    auto graph = std::dynamic_pointer_cast<ComparabilityBigraph>(G);
    return graph->isInV1(v) ? CYAN : RED2;
}

bool DrawableComparabilityBigraph::canCompareFrom(const VertexPointer& v) const {
    auto graph = std::dynamic_pointer_cast<ComparabilityBigraph>(G);
    return graph->isInV2(v);
}

void DrawableComparabilityGraph::drawEdges(QPainter* painter) const {
    auto graph = std::dynamic_pointer_cast<ComparabilityGraph>(G);
    if (graph->getDimension() != 1) {
        DrawableGraph::drawEdges(painter);
    } else { // quadratic bezier curve
        painter->setPen(QPen(Qt::black, 1));
        painter->setBrush(Qt::NoBrush);
        for (const auto& [u, v] : G->enumerate_edges()) {
            const auto [position_u, color_u] = this->vertices.at(u->getId());
            const auto [position_v, color_v] = this->vertices.at(v->getId());
            QPainterPath path;
            auto p1 = remap(position_u);
            auto p2 = remap(position_v);
            path.moveTo(p1);

            double midX = (p1.x() + p2.x()) / 2.0;
            double midY = p1.y();
            double curveHeight = std::abs(p2.x() - p1.x()) * 0.3;

            QPointF controlPoint(midX, midY - curveHeight);

            path.quadTo(controlPoint, p2);
            painter->drawPath(path);
        }
    }
}

void DrawableComparabilityGraph::embedIn0D(){}

void DrawableComparabilityBigraph::embedIn0D() {
    const auto graph = std::dynamic_pointer_cast<ComparabilityBigraph>(G);
    const unsigned p = graph->V1_size();
    const unsigned q = graph->V2_size();

    const double height = graph->getPointSpaceLimit();
    this->point_space_bound = {0, static_cast<unsigned>(height)};

    // divide the space in p or q sections, each one contains a node
    const double zone_size_p = (p > 0) ? height / static_cast<double>(p) : height;
    const double zone_size_q = (q > 0) ? height / static_cast<double>(q) : height;
    double startY_p = zone_size_p / 2.0; // at the center of the section
    double startY_q = zone_size_q / 2.0;
    if (p == 1) startY_p = height / 2.0;
    if (q == 1) startY_q = height / 2.0;

    int p_count = 0;
    int q_count = 0;
    for (const auto& v : graph->enumerate()) {
        if (graph->isInV1(v)) {
            const double y = startY_p + p_count * zone_size_p;
            this->vertices[v->getId()] = {QPointF(height / 4.0, y), getColor(v)};
            p_count++;
        } else {
            const double y = startY_q + q_count * zone_size_q;
            this->vertices[v->getId()] = {QPointF(3.0 * height / 4.0, y), getColor(v)};
            q_count++;
        }
    }
}