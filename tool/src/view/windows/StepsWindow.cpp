#include "StepsWindow.hpp"
#include "../graphicsItems/DrawableGraphNode.hpp"
#include "../graphicsItems/Arrow.hpp"
#include "../graphicsItems/TreeLayout.hpp"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QToolBar>
#include <QPushButton>
#include <QLabel>
#include <cmath>

StepsWindow::StepsWindow(const std::shared_ptr<PartitionTreeNode> &tree, QWidget *parent):
    QMainWindow(parent), next_node_id(0), tree(tree)
{
    setWindowTitle("Steps of the Biclique Decomposition");
    setGeometry(100, 100, 1600, 900);

    setupUI();
    createTree();
}

StepsWindow::~StepsWindow() {
    if (view and view->viewport()) { view->viewport()->removeEventFilter(this);}
}

void StepsWindow::setupUI() {
    scene = std::make_unique<QGraphicsScene>();
    scene->setSceneRect(0, 0, 5000, 4000);
    scene->setBackgroundBrush(QBrush(QColor(64, 63, 75))); //QColor(250, 248, 240)

    view = std::make_unique<QGraphicsView>(scene.get());
    view->setRenderHint(QPainter::Antialiasing);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    view->viewport()->installEventFilter(this);

    auto* toolbar = new QToolBar("Controls", this);
    addToolBar(Qt::TopToolBarArea, toolbar);

    auto* zoomInBtn = new QPushButton("Zoom +", this);
    connect(zoomInBtn, &QPushButton::clicked, [this]() { view->scale(1.3, 1.3); });
    toolbar->addWidget(zoomInBtn);

    auto* zoomOutBtn = new QPushButton("Zoom -", this);
    connect(zoomOutBtn, &QPushButton::clicked, [this]() { view->scale(1.0/1.3, 1.0/1.3); });
    toolbar->addWidget(zoomOutBtn);

    toolbar->addSeparator();

    auto* fitBtn = new QPushButton("Recenter", this);
    connect(fitBtn, &QPushButton::clicked, [this]() {
        const QRectF bounds = scene->itemsBoundingRect();
        view->fitInView(bounds.adjusted(-80, -80, 80, 80), Qt::KeepAspectRatio);
    });
    toolbar->addWidget(fitBtn);
    toolbar->addSeparator();

    auto* infoLabel = new QLabel("Green Leaves belongs to the Biclique Decomposition", this);
    toolbar->addWidget(infoLabel);

    setCentralWidget(view.get());
}

bool StepsWindow::eventFilter(QObject *obj, QEvent *event) {
    if (not view or not view->viewport()) {
        return QMainWindow::eventFilter(obj, event);
    }

    if (obj == view->viewport() and event->type() == QEvent::Wheel) {
        if (const auto* wheelEvent = dynamic_cast<QWheelEvent*>(event); wheelEvent->angleDelta().y() > 0) {
            view->scale(1.15, 1.15);
        } else {
            view->scale(1.0 / 1.15, 1.0 / 1.15);
        }
        return true;
    }

    return QMainWindow::eventFilter(obj, event);
}

DrawableComparabilityBigraphNode* StepsWindow::createNode(int id, const std::shared_ptr<PartitionTreeNode>& construct_info) {
    bool is_terminal = not construct_info->U.has_value() and not construct_info->FL.has_value() and not construct_info->L.has_value();
    auto node = std::make_unique<DrawableComparabilityBigraphNode>(id, is_terminal);
    node->linkGraph(construct_info->graph);

    if (const unsigned dim = construct_info->graph->getDimension(); dim > 0) { //todo and not truncated
        node->addLine(construct_info->H, dim);
    }

    DrawableComparabilityBigraphNode* node_ptr = node.get();
    scene->addItem(node_ptr);
    nodes.push_back(std::move(node));
    return node_ptr;
}

void StepsWindow::createTree() {
    DrawableComparabilityBigraphNode* root = createNode(next_node_id++, tree);
    buildTreeRecursive(root, tree);
    TreeLayout::layoutTree(root, QPointF(1200, 200), 280, 240);
    createArrowsRecursive(root);
    const QRectF all_items_rect = scene->itemsBoundingRect();
    scene->setSceneRect(all_items_rect.adjusted(-150, -150, 150, 150));

    if (all_items_rect.width() < 50000 and all_items_rect.height() < 50000) {
        view->fitInView(all_items_rect.adjusted(-100, -100, 100, 100), Qt::KeepAspectRatio);
    }
}

void StepsWindow::buildTreeRecursive(DrawableComparabilityBigraphNode* parent_node,
                                      const std::shared_ptr<PartitionTreeNode>& tree_node) {
    if (not tree_node or not parent_node) return;

    std::map<QString, DrawableComparabilityBigraphNode*> child_map;
    for (size_t i = 0; i < tree_node->children.size(); ++i) {
        if (auto child_tree_node = tree_node->children[i]) {
            DrawableComparabilityBigraphNode* child_node = createNode(next_node_id++, child_tree_node);
            QString childLabel = QString::fromStdString(child_tree_node->label);
            child_map[childLabel] = child_node;
        }
    }

    for (QStringList labels = {"L", "FL", "U"}; const QString& label : labels) {
        if (child_map.contains(label)) { parent_node->addChild(child_map[label], label); }
    }

    for (const auto& child_tree_node : tree_node->children) {
        if (QString childLabel = QString::fromStdString(child_tree_node->label); child_map.contains(childLabel)) {
            buildTreeRecursive(child_map[childLabel], child_tree_node);
        }
    }
}

void StepsWindow::createArrowsRecursive(DrawableComparabilityBigraphNode* node) {
    if (not node) return;

    QVector<DrawableComparabilityBigraphNode*> children = node->getChildren();
    QVector<QString> labels = node->getChildLabels();

    for (int i = 0; i < children.size(); ++i) {
        auto* arrow = new Arrow(node, children[i], labels[i]);
        scene->addItem(arrow);
        arrow->updatePath();
    }

    for (DrawableComparabilityBigraphNode* child : children) {
        createArrowsRecursive(child);
    }
}