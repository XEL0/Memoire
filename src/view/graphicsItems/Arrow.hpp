#ifndef MEMOIRE_ARROW_HPP
#define MEMOIRE_ARROW_HPP

#include <QGraphicsPathItem>
#include <QGraphicsTextItem>
#include <memory>

class DrawableComparabilityBigraphNode;

class Arrow : public QGraphicsPathItem {
public:
    enum { Type = UserType + 1 };
    
    Arrow(DrawableComparabilityBigraphNode *fromNode, DrawableComparabilityBigraphNode *toNode, const QString &label,
          QGraphicsItem *parent = nullptr);
    
    int type() const override { return Type; }
    
    DrawableComparabilityBigraphNode *startNode() const { return startNode_; }
    DrawableComparabilityBigraphNode *endNode() const { return endNode_; }
    
    void updatePath();
    
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

private:
    DrawableComparabilityBigraphNode *startNode_;
    DrawableComparabilityBigraphNode *endNode_;
    QString label_;
    std::unique_ptr<QGraphicsTextItem> labelItem_;
};

#endif // MEMOIRE_ARROW_HPP
