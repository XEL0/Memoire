#include "DrawableGraphNode.hpp"

DrawableComparabilityBigraphNode::DrawableComparabilityBigraphNode(const unsigned id, const bool isTerminalLeaf, QGraphicsItem *parent):
    DrawableComparabilityBigraph(), parent(nullptr), id(id), is_terminal_leaf(isTerminalLeaf) {
    this->setSceneDimensions(GRAPH_SIZE, GRAPH_SIZE);

    if (is_terminal_leaf) {
        this->setBackgroundColor(QColor(240, 255, 240));
    } else {
        this->setBackgroundColor(Qt::white);
    }
}

void DrawableComparabilityBigraphNode::addChild(DrawableComparabilityBigraphNode *child, const QString &label) {
    if (not children.contains(child)) {
        children.append(child);
        child_labels.append(label);
        child->parent = this;
    }
}

QPointF DrawableComparabilityBigraphNode::getChildConnectionPoint(const int childIndex) const {
    double x;
    if (children.size() == 1) {
        x = GRAPH_SIZE / 2;
    } else if (children.size() == 2) {
        x = (childIndex == 0) ? GRAPH_SIZE * 0.33 : GRAPH_SIZE * 0.67;
    } else {
        x = GRAPH_SIZE * (childIndex + 1) / 4.0; // with a square of size 1, at 0.25, 0.5 and 0.75
    }
    return mapToScene(QPointF(x, GRAPH_SIZE));
}

QPointF DrawableComparabilityBigraphNode::getParentConnectionPoint() const {
    return mapToScene(QPointF(GRAPH_SIZE / 2, 0));
}

void DrawableComparabilityBigraphNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    DrawableComparabilityBigraph::paint(painter, option, widget);
    if (is_terminal_leaf) {
        painter->setPen(QPen(QColor(34, 139, 34), 3));
    } else {
        painter->setPen(QPen(Qt::black, 2));
    }
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(0, 0, GRAPH_SIZE, GRAPH_SIZE);
}