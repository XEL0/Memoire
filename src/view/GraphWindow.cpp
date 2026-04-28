#include "GraphWindow.hpp"

#include <QDateTime>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollBar>

#include "DrawableGraph.hpp"
#include "../model/Algorithms.hpp"


GraphWindow::GraphWindow(
    QMainWindow* creator,
    const std::shared_ptr<DrawableComparabilityBigraph>& drawable,
    const std::shared_ptr<ComparabilityBigraph>& graph): creator(creator), drawable(drawable), graph(graph) {
    setupUI();
}

GraphWindow::~GraphWindow() {
}

void GraphWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setStyleSheet("background-color: white;");
    setCentralWidget(centralWidget);

    setWindowTitle("Comparability Bigraph");
    setGeometry(150, 150, 800, 900);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(10);

    QLabel *canvasLabel = new QLabel("Comparability Bigraph");
    canvasLabel->setStyleSheet("color: black; font-weight: 500; font-size: 12px;");
    mainLayout->addWidget(canvasLabel);

    canvasWidget = new QWidget();
    canvasWidget->setStyleSheet(
        "background-color: white; "
        "border: 1px solid #CCCCCC; "
        "border-radius: 4px;"
    );
    canvasWidget->setMinimumHeight(600);
    canvasWidget->setMinimumWidth(600);

    QVBoxLayout *canvasLayout = new QVBoxLayout(canvasWidget);
    canvasLayout->addWidget(drawable.get());
    mainLayout->addWidget(canvasWidget, 3);

    QLabel *outputLabel = new QLabel("Output:");
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


    QWidget* controlsWidget = new QWidget();
    controlsWidget->setStyleSheet("background-color: white;");
    QGridLayout* gridLayout = new QGridLayout(controlsWidget);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setSpacing(10);
    gridLayout->setColumnStretch(0, 1);
    gridLayout->setColumnStretch(1, 1);
    gridLayout->setColumnStretch(2, 1);


    QWidget *toggleBicliqueCellWidget = new QWidget();
    QHBoxLayout *toggleBicliqueLayout = new QHBoxLayout(toggleBicliqueCellWidget);
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

    connect(showStepsCheckBox, &QCheckBox::toggled, this, &GraphWindow::onOptimizeToggled);

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
            this, &GraphWindow::onComputeBicliqueCoverClicked);

    gridLayout->addWidget(toggleBicliqueCellWidget, 0, 0, 1, 1);


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
    gridLayout->addWidget(showStepsBtn, 0, 2);

    connect(showStepsBtn, &QPushButton::clicked, this, [this]() {
        appendOutput("Steps visualization not yet implemented");
    });

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
    gridLayout->addWidget(computeBFSBtn, 0, 1, 1, 1);

    connect(computeBFSBtn, &QPushButton::clicked, this, &GraphWindow::onComputeBFSClicked);

    mainLayout->addWidget(controlsWidget);
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

void GraphWindow::onComputeBicliqueCoverClicked() {
    appendOutput("");
    appendOutput(">>> Biclique Cover Computation " + QString(optimize ? "with size optimization" : "without size optimization"));
    BicliquePartitioner partitioner = BicliquePartitioner();
    auto p = partitioner.partition(graph, optimize);
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

void GraphWindow::onOptimizeToggled(bool checked) {
    optimize = checked;
}

void GraphWindow::onComputeBFSClicked() {
    appendOutput("Not Implemented");
    //appendOutput("Show Steps: " + QString(showSteps ? "ON" : "OFF"));
}