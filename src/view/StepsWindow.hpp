#ifndef MEMOIRE_STEPSWINDOW_HPP
#define MEMOIRE_STEPSWINDOW_HPP

#include <QMainWindow>
#include <QWidget>
#include <QEvent>
#include <QWheelEvent>
#include <memory>
#include <vector>
#include <QGraphicsScene>

#include "../model/PartitionTreeLoader.hpp"

class DrawableComparabilityBigraphNode;

class StepsWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit StepsWindow(const std::shared_ptr<PartitionTreeNode> &tree, QWidget *parent = nullptr);
    ~StepsWindow() override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void setupUI();
    void createTree();
    void buildTreeRecursive(DrawableComparabilityBigraphNode *parent_node,
                           const std::shared_ptr<PartitionTreeNode>& tree_node);
    void createArrowsRecursive(DrawableComparabilityBigraphNode *node);
    DrawableComparabilityBigraphNode* createNode(int id, const std::shared_ptr<PartitionTreeNode>& construct_info);

    std::unique_ptr<QGraphicsScene> scene;
    std::unique_ptr<QGraphicsView> view;
    std::vector<std::unique_ptr<DrawableComparabilityBigraphNode>> nodes;
    int next_node_id;
    std::shared_ptr<PartitionTreeNode> tree;
};

#endif // MEMOIRE_STEPSWINDOW_HPP