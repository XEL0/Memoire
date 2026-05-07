#ifndef MEMOIRE_TREE_LAYOUT_HPP
#define MEMOIRE_TREE_LAYOUT_HPP

#include <QPoint>
#include <QDebug>
#include <algorithm>
#include "DrawableGraphNode.hpp"

class TreeLayout {
public:
    static void layoutTree(DrawableComparabilityBigraphNode *root,
                          const QPointF root_position,
                          const double vertical_spacing,
                          const double min_horizontal_spacing) {
        if (!root) return;
        layoutNode(root, root_position, vertical_spacing, min_horizontal_spacing);
    }

private:
    static double calculateSubtreeWidth(const DrawableComparabilityBigraphNode *node, const double min_spacing) {
        if (!node) return 0;

        QVector<DrawableComparabilityBigraphNode*> children = node->getChildren();

        if (children.isEmpty()) {
            return 240.0;
        }

        double total_child_width = 0;
        for (const auto child : children) {
            total_child_width += calculateSubtreeWidth(child, min_spacing);
        }

        const double spacing = (children.size() - 1) * min_spacing;
        return std::max(240.0, total_child_width + spacing);
    }

    static void layoutNode(DrawableComparabilityBigraphNode *node,
                          QPointF node_pos,
                          double vertical_spacing,
                          double min_horizontal_spacing) {
        if (!node) return;

        node->setPos(node_pos);
        QVector<DrawableComparabilityBigraphNode*> children = node->getChildren();
        QVector<QString> child_labels = node->getChildLabels();

        if (children.isEmpty()) return;

        QVector<int> sorted_indices;
        for (int i = 0; i < children.size(); ++i) { sorted_indices.append(i); }
        std::ranges::sort(sorted_indices,[&child_labels](const int a, const int b) {
            auto order = [](const QString& label) { return label == "L" ? 0 : label == "FL" ? 1 : 2; };
            return order(child_labels[a]) < order(child_labels[b]);
        });

        QVector<double> child_width;
        for (const int idx : sorted_indices) {
            const double width = calculateSubtreeWidth(children[idx], min_horizontal_spacing);
            child_width.append(width);
        }

        double total_width = 0;
        for (const double w : child_width) { total_width += w; }
        total_width += (child_width.size() - 1) * min_horizontal_spacing;

        const double start_x = node_pos.x() - total_width / 2.0;
        double currentX = start_x;

        for (int i = 0; i < sorted_indices.size(); ++i) {
            const int idx = sorted_indices[i];
            const double width = child_width[i];
            const QPointF child_pos(currentX + width / 2.0, node_pos.y() + vertical_spacing);

            layoutNode(children[idx], child_pos, vertical_spacing, min_horizontal_spacing);

            currentX += width + min_horizontal_spacing;
        }
    }
};

#endif // MEMOIRE_TREE_LAYOUT_HPP