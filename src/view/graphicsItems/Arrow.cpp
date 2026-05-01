#include "Arrow.hpp"
#include "DrawableGraphNode.hpp"
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsScene>
#include <cmath>
#include <QDebug>
#include <utility>

Arrow::Arrow(DrawableComparabilityBigraphNode *fromNode, DrawableComparabilityBigraphNode *toNode, QString label,
            QGraphicsItem *parent):
    QGraphicsPathItem(parent), start_node(fromNode), end_node(toNode), label(std::move(label)){
    setFlag(ItemSendsGeometryChanges);
    setPen(QPen(Qt::white, 2));
    setZValue(-1);
    label_item = nullptr;
    updatePath();
}

Arrow::~Arrow() {}


void Arrow::updatePath() {
    const QVector<DrawableComparabilityBigraphNode*> children = start_node->getChildren();
    QVector<QString> labels = start_node->getChildLabels();
    const int child_index = children.indexOf(end_node);

    QVector<int> sorted_index;
    for (int i = 0; i < children.size(); ++i) { sorted_index.append(i); }
    std::ranges::sort(sorted_index,[&labels](const int a, const int b) {
        auto order = [](const QString& label) { return label == "L" ? 0 : label == "FL" ? 1 : 2; };
        return order(labels[a]) < order(labels[b]);
    });
    int sortedPosition = 0;
    for (int i = 0; i < sorted_index.size(); ++i) {
        if (sorted_index[i] == child_index) {
            sortedPosition = i;
            break;
        }
    }

    const QPointF start_point = start_node->getChildConnectionPoint(sortedPosition);
    const QPointF end_point = end_node->getParentConnectionPoint();

    QPainterPath path;
    path.moveTo(start_point);

    const double dy = end_point.y() - start_point.y();
    const QPointF controlPoint1(start_point.x(), start_point.y() + dy * 0.4);
    const QPointF controlPoint2(end_point.x(), start_point.y() + dy * 0.6);

    path.cubicTo(controlPoint1, controlPoint2, end_point);
    setPath(path);

    if (not label_item and this->scene()) {
        label_item = new QGraphicsTextItem(label);
        label_item->setDefaultTextColor(Qt::white);
        label_item->setFont(QFont("Arial", 20, 500));
        label_item->setZValue(1);
        scene()->addItem(label_item);
    }

    if (label_item) {
        const QPointF mid_point = path.pointAtPercent(0.5);
        label_item->setPos(mid_point + QPointF(10, -25));
    }
}

void Arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QGraphicsPathItem::paint(painter, option, widget);

    // arrowhead
    if (const QPainterPath path_copy = this->path(); path_copy.elementCount() > 1) {
        const QPointF end_point = path_copy.pointAtPercent(1.0);
        const QPointF before_end = path_copy.pointAtPercent(0.95);
        const QPointF dir = end_point - before_end;
        if (dir.manhattanLength() < 0.001) return;
        const double angle = std::atan2(dir.y(), dir.x());
        const double arrow_size = 15;
        const QPointF p1 = end_point;
        const QPointF p2 = end_point + QPointF(-arrow_size * cos(angle - M_PI / 6),
                                        -arrow_size * sin(angle - M_PI / 6));
        const QPointF p3 = end_point + QPointF(-arrow_size * cos(angle + M_PI / 6),
                                        -arrow_size * sin(angle + M_PI / 6));
        painter->setBrush(QBrush(Qt::white));
        painter->setPen(Qt::NoPen);
        painter->drawPolygon(QPolygonF() << p1 << p2 << p3);
    }
}