#ifndef MEMOIRE_STEPSWINDOW_HPP
#define MEMOIRE_STEPSWINDOW_HPP

#include <QMainWindow>
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <memory>
#include <QGraphicsScene>

#include "../model/PartitionTreeLoader.hpp"

class DrawableComparabilityBigraphNode;

class StepsWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit StepsWindow(const std::shared_ptr<PartitionTreeNode> &tree, QWidget *parent = nullptr);
    ~StepsWindow() override;

private:
    void setupUI();
    void createTree();

    void buildTreeRecursive(DrawableComparabilityBigraphNode *parentNode, std::shared_ptr<PartitionTreeNode> treeNode);

    DrawableComparabilityBigraphNode* createNode(int nodeId, std::shared_ptr<PartitionTreeNode> constructInfo);

    std::unique_ptr<QGraphicsScene> scene_;
    std::unique_ptr<QGraphicsView> view_;
    std::vector<std::unique_ptr<DrawableComparabilityBigraphNode>> nodes_;

    int nextNodeId_;
    std::shared_ptr<PartitionTreeNode> tree;
};

#endif // MEMOIRE_STEPSWINDOW_HPP