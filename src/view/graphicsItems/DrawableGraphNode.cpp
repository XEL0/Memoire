#include "DrawableGraphNode.hpp"

DrawableComparabilityBigraphNode::DrawableComparabilityBigraphNode(unsigned id, QGraphicsItem *parent): DrawableComparabilityBigraph(), parent(nullptr), id(id) {

}

void DrawableComparabilityBigraphNode::addChild(DrawableComparabilityBigraphNode *child, const QString &label) {
    if (!children.contains(child)) {
        children.append(child);
        childLabels.append(label);
        child->parent = this;
    }
}

void DrawableComparabilityBigraphNode::removeChild(DrawableComparabilityBigraphNode *child) {
    int index = children.indexOf(child);
    if (index != -1) {
        children.removeAt(index);
        childLabels.removeAt(index);
        child->parent = nullptr;
    }
}

QPointF DrawableComparabilityBigraphNode::getChildConnectionPoint(int childIndex) const {
    double y = -GRAPH_SIZE/2 + childIndex * GRAPH_SIZE/2;
    return mapToScene(QPointF(GRAPH_SIZE/2, y));
}

QPointF DrawableComparabilityBigraphNode::getParentConnectionPoint() const {
    return mapToScene(QPointF(-GRAPH_SIZE/2, 0));
}

QVariant DrawableComparabilityBigraphNode::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionHasChanged) {
    }
    return QGraphicsItem::itemChange(change, value);
}

void DrawableComparabilityBigraphNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setPen(QPen(Qt::black, 1.5));
    painter->setBrush(QBrush(Qt::white));
    painter->drawRect(-GRAPH_SIZE/2, -GRAPH_SIZE/2, GRAPH_SIZE, GRAPH_SIZE);
    DrawableComparabilityBigraph::paint(painter, option, widget);
}
