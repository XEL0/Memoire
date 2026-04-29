#ifndef MEMOIRE_GRAPHNODE_HPP
#define MEMOIRE_GRAPHNODE_HPP
#include "DrawableGraph.hpp"
#include <QGraphicsItem>


class DrawableComparabilityBigraphNode: public DrawableComparabilityBigraph {
protected:
    DrawableComparabilityBigraphNode* parent;
    QVector<DrawableComparabilityBigraphNode*> children;
    QVector<QString> childLabels;
    unsigned id;
    unsigned GRAPH_SIZE = 100;

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
public:
    DrawableComparabilityBigraphNode(unsigned id, QGraphicsItem *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void addChild(DrawableComparabilityBigraphNode *child, const QString &label);
    void removeChild(DrawableComparabilityBigraphNode *child);
    int getId() const { return id; }
    QVector<DrawableComparabilityBigraphNode*> getChildren() const { return children; }
    QVector<QString> getChildLabels() const { return childLabels; }
    DrawableComparabilityBigraphNode* getParent() const { return parent; }
    QPointF getChildConnectionPoint(int childIndex) const;
    QPointF getParentConnectionPoint() const;
};


#endif //MEMOIRE_GRAPHNODE_HPP
