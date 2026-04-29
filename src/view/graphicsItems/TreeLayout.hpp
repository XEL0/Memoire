#ifndef MEMOIRE_TREE_LAYOUT_HPP
#define MEMOIRE_TREE_LAYOUT_HPP

#include <QPoint>
#include "DrawableGraphNode.hpp"

class TreeLayout {
public:
    struct NodePosition {
        DrawableComparabilityBigraphNode *node;
        QPointF position;
    };
    
    static void layoutTree(DrawableComparabilityBigraphNode *root, QPointF rootPosition = QPointF(50, 300)) {
        if (!root) return;
        
        root->setPos(rootPosition);

        const double horizontalSpacing = 200;
        const double verticalSpacing = 100;
        
        layoutNode(root, rootPosition, horizontalSpacing, verticalSpacing);
    }

private:
    static void layoutNode(DrawableComparabilityBigraphNode *node, QPointF parentPos,
                          double horizontalSpacing, double verticalSpacing) {
        
        QVector<DrawableComparabilityBigraphNode*> children = node->getChildren();
        
        if (children.isEmpty()) {
            return;
        }

        double baseY = parentPos.y() - verticalSpacing;
        QPointF positions[] = {
            QPointF(parentPos.x() + horizontalSpacing, baseY),
            QPointF(parentPos.x() + horizontalSpacing, parentPos.y()),
            QPointF(parentPos.x() + horizontalSpacing, baseY + 2*verticalSpacing)
        };

        for (int i = 0; i < children.size() && i < 3; ++i) {
            children[i]->setPos(positions[i]);

            layoutNode(children[i], positions[i], horizontalSpacing, verticalSpacing);
        }
    }
};

#endif // MEMOIRE_TREE_LAYOUT_HPP
