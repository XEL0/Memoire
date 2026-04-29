#include "Arrow.hpp"
#include "DrawableGraphNode.hpp"
#include <QPainter>
#include <QPainterPath>
#include <cmath>

Arrow::Arrow(DrawableComparabilityBigraphNode *fromNode, DrawableComparabilityBigraphNode *toNode, const QString &label,
            QGraphicsItem *parent)
    : QGraphicsPathItem(parent), startNode_(fromNode), endNode_(toNode), label_(label)
{
    setFlag(ItemSendsGeometryChanges);
    setPen(QPen(Qt::black, 2));
    setZValue(0);
    
    // Créer l'étiquette
    labelItem_ = std::make_unique<QGraphicsTextItem>(label);
    labelItem_->setDefaultTextColor(Qt::red);
    labelItem_->setFont(QFont("Arial", 10, QFont::Bold));
    
    updatePath();
}

void Arrow::updatePath() {
    // Obtenir les points de connexion
    QPointF startPoint = startNode_->getChildConnectionPoint(
        startNode_->getChildren().indexOf(endNode_));
    QPointF endPoint = endNode_->getParentConnectionPoint();
    
    // Créer un chemin courbe (Bezier)
    QPainterPath path;
    path.moveTo(startPoint);
    
    // Point de contrôle pour une belle courbe
    QPointF controlPoint((startPoint.x() + endPoint.x()) / 2,
                        (startPoint.y() + endPoint.y()) / 2);
    
    path.cubicTo(controlPoint, controlPoint, endPoint);
    setPath(path);
    
    // Positionner le label au milieu
    if (labelItem_) {
        QPointF midPoint = (startPoint + endPoint) / 2;
        labelItem_->setPos(midPoint + QPointF(10, -10));
    }
}

void Arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                 QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    QGraphicsPathItem::paint(painter, option, widget);
    
    // Dessiner la pointe de flèche
    QPainterPath path = this->path();
    if (path.elementCount() > 0) {
        // Obtenir le dernier point du chemin
        QPointF endPoint = path.currentPosition();
        QPointF prevPoint;
        
        if (path.elementCount() >= 2) {
            int lastIdx = path.elementCount() - 1;
            prevPoint = path.elementAt(lastIdx - 1);
        } else {
            prevPoint = endPoint;
        }
        
        // Calculer l'angle
        QPointF dir = endPoint - prevPoint;
        double angle = std::atan2(dir.y(), dir.x());
        
        // Dessiner la pointe de flèche
        double arrowSize = 15;
        QPointF p1 = endPoint;
        QPointF p2 = endPoint + QPointF(-arrowSize * cos(angle - M_PI / 6),
                                        -arrowSize * sin(angle - M_PI / 6));
        QPointF p3 = endPoint + QPointF(-arrowSize * cos(angle + M_PI / 6),
                                        -arrowSize * sin(angle + M_PI / 6));
        
        painter->setBrush(QBrush(Qt::black));
        painter->drawPolygon(QPolygonF() << p1 << p2 << p3);
    }
}
