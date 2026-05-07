#ifndef MEMOIRE_GRAPHNODE_HPP
#define MEMOIRE_GRAPHNODE_HPP
#include "DrawableGraph.hpp"
#include <QGraphicsItem>


class DrawableComparabilityBigraphNode: public DrawableComparabilityBigraph {
public:
    explicit DrawableComparabilityBigraphNode(unsigned id, bool isTerminalLeaf = false, QGraphicsItem *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void addChild(DrawableComparabilityBigraphNode *child, const QString &label);
    int getId() const { return id; }
    bool isTerminalLeaf() const { return is_terminal_leaf; }
    QVector<DrawableComparabilityBigraphNode*> getChildren() const { return children; }
    QVector<QString> getChildLabels() const { return child_labels; }
    DrawableComparabilityBigraphNode* getParent() const { return parent; }
    QPointF getChildConnectionPoint(int childIndex) const;
    QPointF getParentConnectionPoint() const;

protected:
    DrawableComparabilityBigraphNode* parent;
    QVector<DrawableComparabilityBigraphNode*> children;
    QVector<QString> child_labels;
    unsigned id;
    unsigned GRAPH_SIZE = 200;
    bool is_terminal_leaf;
};


#endif //MEMOIRE_GRAPHNODE_HPP