#include "StepsWindow.hpp"
#include "graphicsItems/DrawableGraphNode.hpp"
#include "graphicsItems/Arrow.hpp"
#include "graphicsItems/TreeLayout.hpp"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#include <QVBoxLayout>
#include <cmath>

StepsWindow::StepsWindow(const std::shared_ptr<PartitionTreeNode> &tree, QWidget *parent):
    QMainWindow(parent), nextNodeId_(0), tree(tree)
{
    setWindowTitle("Ternary Tree with Graphs");
    setGeometry(100, 100, 1400, 800);

    setupUI();
    createTree();  // Appeler createTree() directement
}

StepsWindow::~StepsWindow() = default;

void StepsWindow::setupUI() {
    // Créer la scène et la vue
    scene_ = std::make_unique<QGraphicsScene>();
    scene_->setSceneRect(0, 0, 1400, 800);
    scene_->setBackgroundBrush(QBrush(QColor(250, 248, 240)));

    view_ = std::make_unique<QGraphicsView>(scene_.get());
    view_->setRenderHint(QPainter::Antialiasing);
    view_->setDragMode(QGraphicsView::ScrollHandDrag);
    view_->setOptimizationFlags(QGraphicsView::DontSavePainterState);

    setCentralWidget(view_.get());
}

DrawableComparabilityBigraphNode* StepsWindow::createNode(int nodeId, std::shared_ptr<PartitionTreeNode> constructInfo) {
    auto node = std::make_unique<DrawableComparabilityBigraphNode>(nodeId);
    node->linkGraph(constructInfo->graph);
    node->addLine(constructInfo->H, constructInfo->graph->getDimension()-1);

    DrawableComparabilityBigraphNode* nodePtr = node.get();
    scene_->addItem(nodePtr);
    nodes_.push_back(std::move(node));
    return nodePtr;
}

void StepsWindow::createTree() {
    if (!tree) {
        qDebug() << "ERREUR: tree est nullptr!";
        return;
    }

    qDebug() << "========================================";
    qDebug() << "=== DEBUG createTree() - DÉMARRAGE ===";
    qDebug() << "========================================";

    qDebug() << "\n--- Création de la racine ---";
    DrawableComparabilityBigraphNode* root = createNode(nextNodeId_++, tree);

    qDebug() << "Racine créée (ID:" << (nextNodeId_-1) << ")";
    qDebug() << "  Position:" << root->pos();
    qDebug() << "  BoundingRect:" << root->boundingRect();
    qDebug() << "  SceneBoundingRect:" << root->sceneBoundingRect();

    qDebug() << "\n--- État des enfants du tree logique ---";
    qDebug() << "  tree->U (enfant 0):" << tree->U;
    qDebug() << "  tree->FL (enfant 1):" << tree->FL;
    qDebug() << "  tree->L (enfant 2):" << tree->L;
    qDebug() << "  tree->children.size():" << tree->children.size();

    // VERSION SIMPLIFIÉE: Créer juste le premier enfant U pour déboguer
    DrawableComparabilityBigraphNode* childU = nullptr;

    if (tree->U && tree->children.size() > 0 && tree->children[0]) {
        qDebug() << "\n--- Création de l'enfant U ---";

        childU = createNode(nextNodeId_++, tree->children[0]);

        qDebug() << "Enfant U créé (ID:" << (nextNodeId_-1) << ")";
        qDebug() << "  Position avant addChild:" << childU->pos();
        qDebug() << "  BoundingRect:" << childU->boundingRect();
        qDebug() << "  SceneBoundingRect:" << childU->sceneBoundingRect();

        root->addChild(childU, "U");
        qDebug() << "  ✓ Enfant ajouté au parent";

        Arrow* arrow = new Arrow(root, childU, "U");
        scene_->addItem(arrow);
        qDebug() << "  ✓ Flèche ajoutée à la scène";
    } else {
        qDebug() << "\n⚠ Pas d'enfant U trouvé";
    }

    // Positionnement manuel
    qDebug() << "\n--- Positionnement des nœuds ---";
    root->setPos(100, 300);
    qDebug() << "Racine positionné à (100, 300)";
    qDebug() << "  Position après setPos:" << root->pos();
    qDebug() << "  SceneBoundingRect:" << root->sceneBoundingRect();

    if (childU) {
        QPointF childPos(400, 200);
        childU->setPos(childPos);
        qDebug() << "Enfant U positionné à" << childPos;
        qDebug() << "  SceneBoundingRect:" << childU->sceneBoundingRect();
    }

    // Informations finales
    qDebug() << "\n--- État final de la scène ---";
    qDebug() << "Scene rect:" << scene_->sceneRect();
    qDebug() << "Nombre total d'items:" << scene_->items().size();

    // Afficher tous les items
    for (int i = 0; i < scene_->items().size(); ++i) {
        QGraphicsItem* item = scene_->items()[i];
        qDebug() << "  Item" << i << ":" << item->type()
                 << "pos=" << item->pos()
                 << "boundingRect=" << item->boundingRect();
    }

    // Calculer le rect de tous les items
    QRectF allItemsRect = scene_->itemsBoundingRect();
    qDebug() << "\nItemsBoundingRect (tous les items):" << allItemsRect;
    qDebug() << "  Dimensions: " << allItemsRect.width() << "x" << allItemsRect.height();

    // Appliquer fitInView
    qDebug() << "\n--- FitInView ---";
    qDebug() << "View rect avant fitInView:" << view_->rect();
    view_->fitInView(allItemsRect, Qt::KeepAspectRatio);
    qDebug() << "✓ fitInView appliqué";
    qDebug() << "View rect après fitInView:" << view_->rect();

    // Mettre à jour les flèches
    qDebug() << "\n--- Mise à jour des flèches ---";
    int arrowCount = 0;
    for (QGraphicsItem* item : scene_->items()) {
        Arrow* arrow = dynamic_cast<Arrow*>(item);
        if (arrow) {
            arrow->updatePath();
            arrowCount++;
            qDebug() << "  ✓ Arrow" << arrowCount << "mise à jour";
        }
    }

    qDebug() << "\n========================================";
    qDebug() << "=== createTree() - FIN ===";
    qDebug() << "========================================\n";
}

void StepsWindow::buildTreeRecursive(DrawableComparabilityBigraphNode* parentNode,
                                      std::shared_ptr<PartitionTreeNode> treeNode) {
    // Non utilisé pour l'instant (version debug simplifiée)
}