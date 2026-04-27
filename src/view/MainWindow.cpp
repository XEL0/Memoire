#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QGraphicsDropShadowEffect>
#include <QRandomGenerator>

#include "MainWindow.hpp"
#include "GraphWindow.hpp"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) { setupUI();}

MainWindow::~MainWindow() {}

QWidget* MainWindow::setupCentralWidget() {
    const auto mainCentralWidget = new QWidget(this);
    mainCentralWidget->setStyleSheet("background-color: #FFFFFF;");
    setCentralWidget(mainCentralWidget);
    setWindowTitle("");
    return mainCentralWidget;
}

QWidget* MainWindow::setupHeader() {
    auto *headerWidget = new QWidget();
    headerWidget->setStyleSheet("background-color: #FFFFFF; border: 1px solid #CCCCCC");
    headerWidget->setFixedHeight(80);

    auto *headerLayout = new QVBoxLayout(headerWidget);
    auto *titleLabel = new QLabel("Graph Maker");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #28282D;");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(24);
    titleLabel->setFont(titleFont);
    headerLayout->addWidget(titleLabel);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    return headerWidget;
}

QWidget* MainWindow::setupFooter() {
    auto *footerWidget = new QWidget();
    footerWidget->setStyleSheet("background-color: white;");
    auto *footerLayout = new QVBoxLayout(footerWidget);
    footerLayout->setContentsMargins(30, 20, 30, 30);

    generateBtn = new QPushButton("Generate");
    generateBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #00B0AF; "
        "  padding: 12px; "
        "  font-size: 16px; "
        "  font-weight: 500;"
        "  border-radius: 8px;"
        "  color: #FFFFFF;"
        //"  border: 1px solid #CCCCCC;"
        "}"
        "QPushButton:hover {"
        "  background-color: #009D99;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #008384;"
        "}"
    );
    generateBtn->setFixedHeight(50);
    footerLayout->addWidget(generateBtn);
    return footerWidget;
}

QWidget* MainWindow::setupBody() {
    contentWidget = new QWidget();
    contentWidget->setStyleSheet("background-color: white;");
    const auto contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setSpacing(10);
    contentLayout->setContentsMargins(30, 20, 30, 20);

    const auto typeLabel = new QLabel("Type of Graph");
    typeLabel->setAlignment(Qt::AlignCenter);
    typeLabel->setStyleSheet("color: #28282D;");
    QFont typeFont = typeLabel->font();
    typeFont.setPointSize(12);
    typeLabel->setFont(typeFont);
    contentLayout->addWidget(typeLabel);

    graphTypeCombo = setupGraphTypeComboBox();
    contentLayout->addWidget(graphTypeCombo);

    optionsWidget = new QWidget();
    optionsWidget->setStyleSheet("background-color: white;");
    optionsLayout = new QGridLayout(optionsWidget);
    optionsLayout->setSpacing(15);
    optionsLayout->setContentsMargins(0, 0, 0, 0);

    contentLayout->addWidget(optionsWidget);
    contentLayout->addStretch();
    return contentWidget;
}

QComboBox* MainWindow::setupGraphTypeComboBox() {
    graphTypeCombo = new QComboBox();
    graphTypeCombo->addItems({"Simple", "Bipartite", "Comparability", "Bicomparability"});
    graphTypeCombo->setStyleSheet(
        "QComboBox {"
        "  padding: 8px; "
        "  font-size: 14px; "
        "  background-color: #FFFFFF;"
        "  color: #28282D;"
        "  border: 1px solid #CCCCCC; "
        "  border-radius: 8px;"
        "}"
        "QComboBox::drop-down {"
        "  border: none;"
        "}"
    );
    graphTypeCombo->setFixedHeight(50);
    return graphTypeCombo;
}

void MainWindow::setupUI() {
    QWidget* mainCentralWidget = setupCentralWidget();

    const auto mainLayout = new QVBoxLayout(mainCentralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QWidget* headerWidget = setupHeader();
    mainLayout->addWidget(headerWidget);

    contentWidget = setupBody();
    mainLayout->addWidget(contentWidget, 1);

    QWidget *footerWidget = setupFooter();
    mainLayout->addWidget(footerWidget);


    connect(graphTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onGraphTypeChanged);

    connect(generateBtn, &QPushButton::clicked, this, &MainWindow::onGenerateClicked);

    // Initialiser les options
    onGraphTypeChanged(0);
}

QPushButton* MainWindow::instantiateRandomButton() {
    QPushButton* randomBtn = new QPushButton("🎲");
    randomBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #FFFFFF; "
        "  border: 1px solid #CCCCCC; "
        "  border-radius: 8px;"
        "  color: #28282D;"
        "  font-size: 20px;"
        "  padding: 0px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #EBEBEC;"
        "}"
    );
    randomBtn->setFixedSize(45, 40);
    return randomBtn;
}

QSpinBox* MainWindow::instantiateSpinBox(const QString &type) {
    QSpinBox* spinBox = new QSpinBox();
    spinBox->setStyleSheet(
        "QSpinBox {"
        "  padding: 6px 28px 6px 8px;"
        "  border: 1px solid #CCCCCC;"
        "  border-radius: 8px;"
        "  background-color: #FFFFFF;"
        "  color: #28282D;"
        "}"

        "QSpinBox::up-button, QSpinBox::down-button {"
        "  width: 20px;"
        "  border: none;"
        "  background: transparent;"
        "}"

        "QSpinBox::up-button {"
        "  subcontrol-origin: border;"
        "  subcontrol-position: top right;"
        "}"

        "QSpinBox::down-button {"
        "  subcontrol-origin: border;"
        "  subcontrol-position: bottom right;"
        "}"

        "QSpinBox::up-arrow {"
        "  image: url(../assets/up_arrow.png);"
        "  width: 10px;"
        "  height: 10px;"
        "}"

        "QSpinBox::down-arrow {"
        "  image: url(../assets/down_arrow.png);"
        "  width: 10px;"
        "  height: 10px;"
        "}"

        "QSpinBox::up-button:hover, QSpinBox::down-button:hover {"
        "  border-radius: 8px;"
        "  background-color: #EBEBEC;"
        "}"

        "QSpinBox::up-button:pressed, QSpinBox::down-button:pressed {"
        "  border-radius: 8px;"
        "  background-color: #CCCCCC;"
        "}"
    );
    spinBox->setFixedHeight(40);

    if (type == "vertices") {
        spinBox->setMinimum(1);
        spinBox->setMaximum(99999);
        spinBox->setValue(5);
    } else {  // dimensions
        spinBox->setMinimum(0);
        spinBox->setMaximum(20);
        spinBox->setValue(2);
    }
    return spinBox;
}

std::pair<QPushButton*, QSpinBox*> MainWindow::instantiateIntegerBox(const QString &type, const QString& label) {
    QPushButton* randomBtn = instantiateRandomButton();
    QSpinBox* spinBox = instantiateSpinBox(type);

    connect(randomBtn, &QPushButton::clicked, [spinBox, type]() {
        int randomValue;
        if (type == "vertices") {
            randomValue = QRandomGenerator::global()->bounded(1, 100);
        } else {  // dimensions
            randomValue = QRandomGenerator::global()->bounded(1, 20);
        }
        spinBox->setValue(randomValue);
    });

    randomButtons[label] = randomBtn;
    spinBoxes[label] = spinBox;
    return {randomBtn, spinBox};
}

QPushButton* MainWindow::instantiateToggleButton(const QString &label) {

    auto* toggleBtn = new QPushButton("False");
    toggleBtn->setCheckable(true);
    toggleBtn->setChecked(false);
    toggleBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #FFFFFF; "
        "  border: 1px solid #CCCCCC; "
        "  border-radius: 8px;"
        "  color: #28282D;"
        "  padding: 8px;"
        "  min-width: 100px;"
        "}"
        "QPushButton:checked {"
        "  background-color: #FFFFFF; "
        "  border: 1px solid #CCCCCC;"
        "}"
        "QPushButton:hover {"
        "  background-color: #EBEBEC;"
        "}"
        "QPushButton:checked:hover {"
        "  background-color: #EBEBEC;"
        "}"
    );
    toggleBtn->setFixedHeight(40);

    connect(toggleBtn, &QPushButton::toggled, [toggleBtn](bool checked) {
        toggleBtn->setText(checked ? "True" : "False");
});

toggleButtons[label] = toggleBtn;
return toggleBtn;
}

void MainWindow::onGraphTypeChanged(int index) {
    QString selectedType = graphTypeCombo->itemText(index);
    populateGraphOptions(selectedType);
}

void MainWindow::populateGraphOptions(const QString &graphType) {
    clearOptions();
    if (not graphConfigs.contains(graphType)) return;

    auto [labels, types] = graphConfigs[graphType];

    for (int i = 0; i < labels.size(); ++i) {
        QString label = labels[i];
        QString type = types[i];

        const int col = i % 2;
        const int labelRow = (i / 2) * 2;

        QLabel *categoryLabel = new QLabel(label);
        categoryLabel->setAlignment(Qt::AlignCenter);
        categoryLabel->setStyleSheet("color: #28282D;");
        QFont catFont = categoryLabel->font();
        catFont.setPointSize(10);
        categoryLabel->setFont(catFont);
        optionsLayout->addWidget(categoryLabel, labelRow, col);

        QWidget *controlWidget = new QWidget();
        QHBoxLayout *controlLayout = new QHBoxLayout(controlWidget);
        controlLayout->setContentsMargins(0, 0, 0, 0);
        controlLayout->setSpacing(8);

        if (type == "vertices" or type == "dimensions") {
            auto [randomBtn, spinBox] = instantiateIntegerBox(type, label);
            controlLayout->addWidget(randomBtn);
            controlLayout->addWidget(spinBox, 1);
            controlLayout->addStretch();
        } else if (type == "toggle") {
            QPushButton *toggleBtn = instantiateToggleButton(label);
            controlLayout->addWidget(toggleBtn);
            controlLayout->addStretch();
        }
        optionsLayout->addWidget(controlWidget, labelRow + 1, col);
    }
}

void MainWindow::clearOptions() {
    QLayoutItem *item;
    while ((item = optionsLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    spinBoxes.clear();
    toggleButtons.clear();
    randomButtons.clear();
}

QMap<QString, QVariant> MainWindow::getFormParameters() const {
    QMap<QString, QVariant> params;

    // Type de graphe sélectionné
    params["graphType"] = graphTypeCombo->currentText();

    // Récupérer tous les SpinBox
    for (auto it = spinBoxes.begin(); it != spinBoxes.end(); ++it) {
        params[it.key()] = it.value()->value();
    }

    // Récupérer tous les Toggles
    for (auto it = toggleButtons.begin(); it != toggleButtons.end(); ++it) {
        params[it.key()] = it.value()->isChecked();
    }

    return params;
}

void MainWindow::onGenerateClicked() {
    const auto params = getFormParameters();
    int size = 900;


    QString graphType = params["graphType"].toString();
    for (auto it = spinBoxes.begin(); it != spinBoxes.end(); ++it) {
        std::cout << it.key().toStdString() << " : " << std::to_string(it.value()->value()) << std::endl;
    }


    if (graphType == "Simple") {
        int vertices = params["Vertices"].toInt();
        bool complete = params["Complete"].toBool();

        const auto graph = std::make_shared<Graph>();
        //graph->generate(vertices, complete);
    } else if (graphType == "Bipartite") {
        int blueVertices = params["Blue Vertices"].toInt();
        int redVertices = params["Red Vertices"].toInt();
        bool complete = params["Complete"].toBool();
        const auto graph = std::make_shared<Bigraph>();

    } else if (graphType == "Comparability") {
        int vertices = params["Vertices"].toInt();
        int dimensions = params["Dimensions"].toInt();
        const auto graph = std::make_shared<ComparabilityGraph>();
        graph->generate(vertices, dimensions, vertices * 100);

    } else if (graphType == "Bicomparability") {
        int blueVertices = params["Blue Vertices"].toInt();
        int redVertices = params["Red Vertices"].toInt();
        int dimensions = params["Dimensions"].toInt();
        const auto graph = std::make_shared<ComparabilityBigraph>();
        graph->generate(blueVertices, redVertices, dimensions, (blueVertices + redVertices) * 100);

        auto *graphWindow = new GraphWindow(this, std::make_shared<DrawableComparabilityBigraph>());
        graphWindow->setAttribute(Qt::WA_DeleteOnClose);
        graphWindow->show();
        this->hide();

        DrawableComparabilityBigraph drawableGraph(graphWindow);
        drawableGraph.resize(size, size);
        drawableGraph.linkGraph(graph);
    }

}