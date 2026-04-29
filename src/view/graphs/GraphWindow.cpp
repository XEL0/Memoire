#include "GraphWindow.hpp"

#include <QDateTime>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollBar>

#include "../widgets/DrawableGraph.hpp"
#include "../StepsWindow.hpp"
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

void GraphWindow::setupCanvas(QVBoxLayout* mainLayout) {
    auto* headerLayout = new QHBoxLayout();
    auto* canvasLabel = new QLabel(title);
    canvasLabel->setStyleSheet("color: black; font-weight: 500; font-size: 12px;");

    mainWindowVisibilityManager = new QPushButton("+");
    mainWindowVisibilityManager->setStyleSheet(
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

    headerLayout->addWidget(canvasLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(mainWindowVisibilityManager);
    connect(mainWindowVisibilityManager, &QPushButton::clicked,
        this, &ComparabilityBigraphWindow::onReShowMainWindow);

    mainLayout->addLayout(headerLayout);

    canvasWidget = new QWidget();
    canvasWidget->setStyleSheet(
        "background-color: white; "
        "border: 1px solid #CCCCCC; "
        "border-radius: 4px;"
    );
    canvasWidget->setMinimumHeight(600);
    canvasWidget->setMinimumWidth(600);

    auto* canvasLayout = new QVBoxLayout(canvasWidget);
    canvasLayout->addWidget(drawable.get());
    mainLayout->addWidget(canvasWidget, 3);
}

void GraphWindow::setupConsole(QVBoxLayout* mainLayout) {
    auto* outputLabel = new QLabel("Output:");
    outputLabel->setStyleSheet("color: black; font-weight: 500; font-size: 12px;");
    mainLayout->addWidget(outputLabel);

    outputTextEdit = new QTextEdit();
    outputTextEdit->setReadOnly(true);
    outputTextEdit->setStyleSheet(
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
    outputTextEdit->setMinimumHeight(120);

    mainLayout->addWidget(outputTextEdit, 1);
}

void GraphWindow::setupButtonsArea(QVBoxLayout* mainLayout) {
    auto* controlsWidget = new QWidget();
    controlsWidget->setStyleSheet("background-color: white;");
    auto* gridLayout = new QGridLayout(controlsWidget);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setSpacing(10);
    gridLayout->setColumnStretch(0, 1);
    gridLayout->setColumnStretch(1, 1);
    gridLayout->setColumnStretch(2, 1);
    setupButtons(gridLayout);
    mainLayout->addWidget(controlsWidget);
}

void GraphWindow::setupButtons(QGridLayout* grid_layout) {}

void ComparabilityBigraphWindow::setupButtons(QGridLayout* grid_layout) {
    setupComputeBicliqueCover(grid_layout);
    setupShowSteps(grid_layout);
    setupComputeBFS(grid_layout);
}

void ComparabilityBigraphWindow::setupComputeBicliqueCover(QGridLayout* grid_layout) {
    auto *toggleBicliqueCellWidget = new QWidget();
    auto *toggleBicliqueLayout = new QHBoxLayout(toggleBicliqueCellWidget);
    toggleBicliqueLayout->setContentsMargins(0, 0, 0, 0);
    toggleBicliqueLayout->setSpacing(8);

    showStepsCheckBox = new QCheckBox();
    showStepsCheckBox->setStyleSheet(
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
    showStepsCheckBox->setChecked(false);
    showStepsCheckBox->setFixedSize(40, 40);
    toggleBicliqueLayout->addWidget(showStepsCheckBox);

    connect(showStepsCheckBox, &QCheckBox::toggled, this, &ComparabilityBigraphWindow::onOptimizeToggled);

    computeBicliqueCoverBtn = new QPushButton("Compute biclique cover");
    computeBicliqueCoverBtn->setStyleSheet(
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
    computeBicliqueCoverBtn->setMinimumHeight(40);
    toggleBicliqueLayout->addWidget(computeBicliqueCoverBtn, 1);

    connect(computeBicliqueCoverBtn, &QPushButton::clicked,
            this, &ComparabilityBigraphWindow::onComputeBicliqueCoverClicked);

    grid_layout->addWidget(toggleBicliqueCellWidget, 0, 0, 1, 1);
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
    setGeometry(150, 150, 800, 900);

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
    outputTextEdit->append(formattedText);
    outputTextEdit->verticalScrollBar()->setValue(outputTextEdit->verticalScrollBar()->maximum());
}

void GraphWindow::clearOutput() {
    outputTextEdit->clear();
}

void ComparabilityBigraphWindow::onComputeBicliqueCoverClicked() {
    auto g = std::dynamic_pointer_cast<ComparabilityBigraph>(graph);
    appendOutput("");
    appendOutput(">>> Biclique Cover Computation " + QString(optimize ? "with size optimization" : "without size optimization"));
    BicliquePartitioner partitioner = BicliquePartitioner();
    auto p = partitioner.partition(g, optimize);
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
    /*auto *stepsWindow = new StepsWindow(PartitionTreeLoader::loadPartitionTreeFromJson("partition_tree.json"));
    stepsWindow->setAttribute(Qt::WA_DeleteOnClose);
    stepsWindow->show();*/
}

void GraphWindow::onReShowMainWindow() {
    if (is_creator_displayed) {
        creator->hide();
        mainWindowVisibilityManager->setText("+");
    } else {
        creator->show();
        mainWindowVisibilityManager->setText("-");
    }
    is_creator_displayed = not is_creator_displayed;
}