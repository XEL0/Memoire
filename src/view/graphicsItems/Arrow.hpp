#ifndef MEMOIRE_ARROW_HPP
#define MEMOIRE_ARROW_HPP

#include <QGraphicsPathItem>
#include <QGraphicsTextItem>

class DrawableComparabilityBigraphNode;

class Arrow : public QGraphicsPathItem {
public:
    Arrow(DrawableComparabilityBigraphNode *fromNode,
          DrawableComparabilityBigraphNode *toNode,
          QString label,
          QGraphicsItem *parent = nullptr);
    ~Arrow() override;

    DrawableComparabilityBigraphNode *startNode() const { return start_node; }
    DrawableComparabilityBigraphNode *endNode() const { return end_node; }
    void updatePath();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
private:
    DrawableComparabilityBigraphNode *start_node;
    DrawableComparabilityBigraphNode *end_node;
    QString label;
    QGraphicsTextItem *label_item;
};

#endif // MEMOIRE_ARROW_HPP