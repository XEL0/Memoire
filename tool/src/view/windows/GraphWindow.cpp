#include "GraphWindow.hpp"

#include <QDateTime>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollBar>
#include <QGraphicsScene>
#include <QGraphicsView>

#include "../graphicsItems/DrawableGraph.hpp"
#include "StepsWindow.hpp"
#include "../../model/PartitionTreeLoader.hpp"
#include "../../model/Algorithms.hpp"


GraphWindow::GraphWindow(QMainWindow *creator, const std::shared_ptr<DrawableGraph> &drawable, const std::shared_ptr<Graph> &graph):
    creator(creator), drawable(drawable), graph(graph), is_creator_displayed(false)
{
    title = "Graph of " + QString::number(graph->size()) + " vertices";
    window_title = "G" + QDateTime::currentDateTime().toString("dd/MM-hh:mm:ss");
    setupUI();
}

BigraphWindow::BigraphWindow(QMainWindow *creator, const std::shared_ptr<DrawableBigraph> &drawable, const std::shared_ptr<Bigraph> &graph):
    GraphWindow(creator, drawable, graph){
        title = "Bigraph of " + QString::number(graph->V1_size()) + " + " + QString::number(graph->V2_size()) +" vertices";
        window_title = "BG" + QDateTime::currentDateTime().toString("dd/MM-hh:mm:ss");
        setupUI();
}

ComparabilityGraphWindow::ComparabilityGraphWindow(
    QMainWindow* creator,
    const std::shared_ptr<DrawableComparabilityGraph>& drawable,
    const std::shared_ptr<ComparabilityGraph>& graph) :
        GraphWindow(creator, drawable, graph){
    title = "Comparability Graph of " + QString::number(graph->size()) + " vertices";
    window_title = "CG" + QDateTime::currentDateTime().toString("dd/MM-hh:mm:ss");
    setupUI();
}

ComparabilityBigraphWindow::ComparabilityBigraphWindow(
    QMainWindow* creator,
    const std::shared_ptr<DrawableComparabilityBigraph>& drawable,
    const std::shared_ptr<ComparabilityBigraph>& graph) :
        GraphWindow(creator, drawable, graph){
    title = "Comparability Bigraph of " + QString::number(graph->V1_size()) + " + " + QString::number(graph->V2_size()) +" vertices";
    window_title = "CBG" + QDateTime::currentDateTime().toString("dd/MM-hh:mm:ss");
    setupUI();
}

GraphWindow::~GraphWindow() = default;
BigraphWindow::~BigraphWindow() = default;
ComparabilityGraphWindow::~ComparabilityGraphWindow() = default;
ComparabilityBigraphWindow::~ComparabilityBigraphWindow() = default;

void GraphWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    const QSize viewSize = graphics_view->viewport()->size();
    drawable->setSceneDimensions(viewSize.width(), viewSize.height());
    graphics_scene->setSceneRect(drawable->boundingRect());
}

void GraphWindow::setupCanvas(QVBoxLayout* main_layout) {
    auto* headerLayout = new QHBoxLayout();
    auto* canvas_label = new QLabel(title);
    canvas_label->setStyleSheet("color: black; font-weight: 500; font-size: 12px;");

    main_window_visibility_manager = new QPushButton("+");
    main_window_visibility_manager->setStyleSheet(
        "QPushButton {"
        "  background-color: white; "
        "  color: black; "
        "  padding: 2px; "
        "  border-radius: 4px; "
        "  font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #F5F5F5;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #E8E8E8;"
        "}"
    );

    headerLayout->addWidget(canvas_label);
    headerLayout->addStretch();
    headerLayout->addWidget(main_window_visibility_manager);
    connect(main_window_visibility_manager, &QPushButton::clicked,
        this, &GraphWindow::onReShowMainWindow);

    main_layout->addLayout(headerLayout);

    canvas_widget = new QWidget();
    canvas_widget->setStyleSheet(
        "background-color: white; "
        "border: 1px solid #CCCCCC; "
        "border-radius: 4px;"
    );
    canvas_widget->setMinimumHeight(600);
    canvas_widget->setMinimumWidth(600);

    auto* canvasLayout = new QVBoxLayout(canvas_widget);
    canvasLayout->setContentsMargins(0, 0, 0, 0);

    graphics_scene = new QGraphicsScene(this);
    graphics_scene->setBackgroundBrush(QBrush(Qt::white));

    graphics_view = new QGraphicsView(graphics_scene, this);
    graphics_view->setRenderHint(QPainter::Antialiasing);

    graphics_scene->addItem(drawable.get());
    graphics_view->resize(600, 600);
    //graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

    canvasLayout->addWidget(graphics_view);
    main_layout->addWidget(canvas_widget, 3);
}

void GraphWindow::setupConsole(QVBoxLayout* main_layout) {
    auto* output_label = new QLabel("Output:");
    output_label->setStyleSheet("color: black; font-weight: 500; font-size: 12px;");
    main_layout->addWidget(output_label);

    output_text_edit = new QTextEdit();
    output_text_edit->setReadOnly(true);
    output_text_edit->setStyleSheet(
        "QTextEdit {"
        "  background-color: #F0F0F0; "
        "  color: #333333; "
        "  border: 1px solid #CCCCCC; "
        "  border-radius: 4px; "
        "  padding: 8px; "
        "  font-family: 'Courier New', monospace; "
        "  font-size: 11px;"
        "}"
    );
    output_text_edit->setMinimumHeight(120);

    main_layout->addWidget(output_text_edit, 1);
}

void GraphWindow::setupButtonsArea(QVBoxLayout* main_layout) {
    auto* controls_widget = new QWidget();
    controls_widget->setStyleSheet("background-color: white;");
    auto* grid_layout = new QGridLayout(controls_widget);
    grid_layout->setContentsMargins(0, 0, 0, 0);
    grid_layout->setSpacing(10);
    grid_layout->setColumnStretch(0, 1);
    grid_layout->setColumnStretch(1, 1);
    grid_layout->setColumnStretch(2, 1);
    setupButtons(grid_layout);
    main_layout->addWidget(controls_widget);
}

void GraphWindow::setupButtons(QGridLayout* grid_layout) {}

void ComparabilityBigraphWindow::setupButtons(QGridLayout* grid_layout) {
    setupComputeBicliqueCover(grid_layout);
    setupShowSteps(grid_layout);
    setupComputeBFS(grid_layout);
}

void ComparabilityBigraphWindow::setupComputeBicliqueCover(QGridLayout* grid_layout) {
    auto *toggle_biclique_cell_widget = new QWidget();
    auto *toggle_biclique_layout = new QHBoxLayout(toggle_biclique_cell_widget);
    toggle_biclique_layout->setContentsMargins(0, 0, 0, 0);
    toggle_biclique_layout->setSpacing(8);

    optimize_size_button = new QCheckBox();
    optimize_size_button->setStyleSheet(
        "QCheckBox {"
        "  width: 40px; "
        "  height: 40px; "
        "}"
        "QCheckBox::indicator {"
        "  width: 40px; "
        "  height: 40px; "
        "  border: 1px solid #CCCCCC; "
        "  border-radius: 4px; "
        "  background-color: #E0E0E0; "
        "}"
        "QCheckBox::indicator:checked {"
        "  background-color: #00B0AF; "
        "}"
    );
    optimize_size_button->setChecked(false);
    optimize_size_button->setFixedSize(40, 40);
    toggle_biclique_layout->addWidget(optimize_size_button);

    connect(optimize_size_button, &QCheckBox::toggled, this, &ComparabilityBigraphWindow::onOptimizeToggled);

    compute_biclique_cover_btn = new QPushButton("Compute biclique cover");
    compute_biclique_cover_btn->setStyleSheet(
        "QPushButton {"
        "  background-color: white; "
        "  color: black; "
        "  border: 1px solid #CCCCCC; "
        "  padding: 10px; "
        "  border-radius: 4px; "
        "  font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #F5F5F5;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #E8E8E8;"
        "}"
    );
    compute_biclique_cover_btn->setMinimumHeight(40);
    toggle_biclique_layout->addWidget(compute_biclique_cover_btn, 1);

    connect(compute_biclique_cover_btn, &QPushButton::clicked,
            this, &ComparabilityBigraphWindow::onComputeBicliqueCoverClicked);

    grid_layout->addWidget(toggle_biclique_cell_widget, 0, 0, 1, 1);
}

void ComparabilityBigraphWindow::setupShowSteps(QGridLayout* grid_layout) {
    showStepsBtn = new QPushButton("Biclique Cover Details");
    showStepsBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: white; "
        "  color: black; "
        "  border: 1px solid #CCCCCC; "
        "  padding: 10px; "
        "  border-radius: 4px; "
        "  font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #F5F5F5;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #E8E8E8;"
        "}"
    );
    showStepsBtn->setMinimumHeight(40);
    grid_layout->addWidget(showStepsBtn, 0, 2);

    connect(showStepsBtn, &QPushButton::clicked, this, &ComparabilityBigraphWindow::onShowStepsClicked);
}

void ComparabilityBigraphWindow::setupComputeBFS(QGridLayout* grid_layout) {
    computeBFSBtn = new QPushButton("Compute BFS");
    computeBFSBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: white; "
        "  color: black; "
        "  border: 1px solid #CCCCCC; "
        "  padding: 10px; "
        "  border-radius: 4px; "
        "  font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #F5F5F5;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #E8E8E8;"
        "}"
    );
    computeBFSBtn->setMinimumHeight(40);
    grid_layout->addWidget(computeBFSBtn, 0, 1, 1, 1);

    connect(computeBFSBtn, &QPushButton::clicked, this, &ComparabilityBigraphWindow::onComputeBFSClicked);
}

void GraphWindow::setupUI() {
    auto *centralWidget = new QWidget(this);
    centralWidget->setStyleSheet("background-color: white;");
    setCentralWidget(centralWidget);

    setWindowTitle(window_title);
    setGeometry(150, 150, 0, 0);

    auto *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(10);

    setupCanvas(mainLayout);
    setupConsole(mainLayout);
    setupButtonsArea(mainLayout);

    appendOutput("Graph Created. Apply algorithms by pressing buttons");
}

void GraphWindow::appendOutput(const QString &text) const {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString formattedText = QString("[%1] %2").arg(timestamp, text);
    output_text_edit->append(formattedText);
    output_text_edit->verticalScrollBar()->setValue(output_text_edit->verticalScrollBar()->maximum());
}

void GraphWindow::clearOutput() {
    output_text_edit->clear();
}

void ComparabilityBigraphWindow::onComputeBicliqueCoverClicked() {
    auto g = std::dynamic_pointer_cast<ComparabilityBigraph>(graph);
    appendOutput("");
    appendOutput(">>> Biclique Cover Computation " + QString(optimize ? "with size optimization" : "without size optimization"));
    auto p = BicliquePartitioner::partition(g, optimize);
    appendOutput(QString(">>> Biclique cover of size %1 found").arg(p.size()));
    for (const auto& G: p) {
        auto blue = std::vector<QString>();
        auto red = std::vector<QString>();
        for (const auto& v : G->enumerate()) {
            if (G->isInV1(v)) {
                blue.push_back(QString::number(v->getId()));
            } else {
                red.push_back(QString::number(v->getId()));
            }
        }
        QString msg = "(<span style='color:blue;'>";
        if (not blue.empty()) {
            msg += blue[0];
            for (size_t i = 1; i < blue.size(); ++i) {
                msg += ", " + blue[i];
            }
        }
        msg += "</span>)◀-▶(<span style='color:red;'>";
        if (not red.empty()) {
            msg += red[0];
            for (size_t i = 1; i < red.size(); ++i) {
                msg += ", " + red[i];
            }
        }
        msg += "</span>)";
        appendOutput(msg);
    }
}

void ComparabilityBigraphWindow::onOptimizeToggled(bool checked) {
    optimize = checked;
}

void ComparabilityBigraphWindow::onComputeBFSClicked() {
    appendOutput("Not Implemented");
    //appendOutput("Show Steps: " + QString(showSteps ? "ON" : "OFF"));
}

void ComparabilityBigraphWindow::onShowStepsClicked() {
    appendOutput(">>> Open a new Window with execution details");
    auto *stepsWindow = new StepsWindow(PartitionTreeLoader::loadPartitionTreeFromJson("partition_tree.json"));
    //PartitionTreeLoader::printPartitionTree(tree);
    stepsWindow->setAttribute(Qt::WA_DeleteOnClose);
    stepsWindow->show();
}

void GraphWindow::onReShowMainWindow() {
    if (is_creator_displayed) {
        creator->hide();
        main_window_visibility_manager->setText("+");
    } else {
        creator->show();
        main_window_visibility_manager->setText("-");
    }
    is_creator_displayed = not is_creator_displayed;
}



TerrainWindow::TerrainWindow(QMainWindow *creator, const std::shared_ptr<DrawableTerrainVisibilityGraph>& drawable,
        const std::shared_ptr<TerrainVisibilityGraph>& visibility_graph, const std::shared_ptr<Terrain>& terrain):
    GraphWindow(creator, drawable, visibility_graph), drawable_visibility_graph(drawable), visibility_graph(visibility_graph), terrain(terrain)
{
    graph = visibility_graph;
    title = "Terrain Visibility Graph of " + QString::number(visibility_graph->size()) + " vertices";
    window_title = "TVG" + QDateTime::currentDateTime().toString("dd/MM-hh:mm:ss");
    setupUI();
}

TerrainWindow::~TerrainWindow() = default;

void TerrainWindow::setupButtons(QGridLayout* grid_layout) {
    setupToggleView(grid_layout);
    setupComputeBicliqueCover(grid_layout);
}

void TerrainWindow::setupToggleView(QGridLayout* grid_layout) {
    toggle_view_btn = new QPushButton("Show VG as Terrain");
    toggle_view_btn->setStyleSheet(
        "QPushButton {"
        "  background-color: white; "
        "  color: black; "
        "  border: 1px solid #CCCCCC; "
        "  padding: 10px; "
        "  border-radius: 4px; "
        "  font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #F5F5F5;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #E8E8E8;"
        "}"
    );
    toggle_view_btn->setMinimumHeight(40);
    grid_layout->addWidget(toggle_view_btn, 0, 0, 1, 1);
    connect(toggle_view_btn, &QPushButton::clicked, this, &TerrainWindow::onToggleViewClicked);
}

void TerrainWindow::setupComputeBicliqueCover(QGridLayout* grid_layout) {
    auto *toggle_biclique_cell_widget = new QWidget();
    auto *toggle_biclique_layout = new QHBoxLayout(toggle_biclique_cell_widget);
    toggle_biclique_layout->setContentsMargins(0, 0, 0, 0);
    toggle_biclique_layout->setSpacing(8);

    optimize_size_button = new QCheckBox();
    optimize_size_button->setStyleSheet(
        "QCheckBox {"
        "  width: 40px; "
        "  height: 40px; "
        "}"
        "QCheckBox::indicator {"
        "  width: 40px; "
        "  height: 40px; "
        "  border: 1px solid #CCCCCC; "
        "  border-radius: 4px; "
        "  background-color: #E0E0E0; "
        "}"
        "QCheckBox::indicator:checked {"
        "  background-color: #00B0AF; "
        "}"
    );
    optimize_size_button->setChecked(false);
    optimize_size_button->setFixedSize(40, 40);
    toggle_biclique_layout->addWidget(optimize_size_button);

    connect(optimize_size_button, &QCheckBox::toggled, this, &TerrainWindow::onOptimizeToggled);

    compute_biclique_cover_btn = new QPushButton("Compute biclique cover");
    compute_biclique_cover_btn->setStyleSheet(
        "QPushButton {"
        "  background-color: white; "
        "  color: black; "
        "  border: 1px solid #CCCCCC; "
        "  padding: 10px; "
        "  border-radius: 4px; "
        "  font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #F5F5F5;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #E8E8E8;"
        "}"
    );
    compute_biclique_cover_btn->setMinimumHeight(40);
    toggle_biclique_layout->addWidget(compute_biclique_cover_btn, 1);

    connect(compute_biclique_cover_btn, &QPushButton::clicked, this, &TerrainWindow::onComputeBicliqueCoverClicked);
    grid_layout->addWidget(toggle_biclique_cell_widget, 0, 1, 1, 1);
}

void TerrainWindow::onToggleViewClicked() {
    updateView();
}

void TerrainWindow::onComputeBicliqueCoverClicked() {
    auto g = std::dynamic_pointer_cast<TerrainVisibilityGraph>(graph);
    appendOutput("");
    appendOutput(">>> Biclique Cover Computation " + QString(optimize ? "with size optimization" : "without size optimization"));
    auto p = CappedGraphDecomposition::decompose(g);
    appendOutput(QString(">>> Biclique cover of size %1 found").arg(p.size()));
    for (const auto& G: p) {
        auto blue = std::vector<QString>();
        auto red = std::vector<QString>();
        for (const auto& v : G->enumerate()) {
            if (G->isInV1(v)) {
                blue.push_back(QString::number(v->getId()));
            } else {
                red.push_back(QString::number(v->getId()));
            }
        }
        QString msg = "(<span style='color:blue;'>";
        if (not blue.empty()) {
            msg += blue[0];
            for (size_t i = 1; i < blue.size(); ++i) {
                msg += ", " + blue[i];
            }
        }
        msg += "</span>)◀-▶(<span style='color:red;'>";
        if (not red.empty()) {
            msg += red[0];
            for (size_t i = 1; i < red.size(); ++i) {
                msg += ", " + red[i];
            }
        }
        msg += "</span>)";
        appendOutput(msg);
    }
}

void TerrainWindow::onOptimizeToggled(const bool checked) {
    optimize = checked;
}

void TerrainWindow::updateView() {
    showing_terrain = not showing_terrain;
    toggle_view_btn->setText(showing_terrain ? "Non-ordered Display": "Display As Terrain");
    drawable_visibility_graph->drawAsTerrain(showing_terrain);
    const QSize viewSize = graphics_view->viewport()->size();
    drawable->setSceneDimensions(viewSize.width(), viewSize.height());
    graphics_scene->setSceneRect(drawable->boundingRect());
    graphics_scene->update();
}