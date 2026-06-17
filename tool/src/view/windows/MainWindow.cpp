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
    graphTypeCombo->addItems(graphTypes);
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
        "  image: url(assets/up_arrow.png);"
        "  width: 10px;"
        "  height: 10px;"
        "}"

        "QSpinBox::down-arrow {"
        "  image: url(assets/down_arrow.png);"
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
        spinBox->setMaximum(500);
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
            randomValue = QRandomGenerator::global()->bounded(1, 3);
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

    params["graphType"] = graphTypeCombo->currentText();

    for (auto it = spinBoxes.begin(); it != spinBoxes.end(); ++it) {
        params[it.key()] = it.value()->value();
    }

    for (auto it = toggleButtons.begin(); it != toggleButtons.end(); ++it) {
        params[it.key()] = it.value()->isChecked();
    }

    return params;
}

void MainWindow::onGenerateClicked() {
    const auto params = getFormParameters();


    QString graphType = params["graphType"].toString();
    /*for (auto it = spinBoxes.begin(); it != spinBoxes.end(); ++it) {
        std::cout << it.key().toStdString() << " : " << std::to_string(it.value()->value()) << std::endl;
    }*/


    if (graphType == "Simple") {
        int vertices = params["Vertices"].toInt();
        bool complete = params["Complete"].toBool();

        const auto graph = std::make_shared<Graph>();
        graph->generate(vertices);
        graph->constructE(complete);

        auto drawableGraph = std::make_shared<DrawableGraph>();
        drawableGraph->linkGraph(graph);

        auto *graphWindow = new GraphWindow(this, drawableGraph, graph);
        graphWindow->setAttribute(Qt::WA_DeleteOnClose);
        graphWindow->show();
        this->hide();

    } else if (graphType == "Bipartite") {
        int blueVertices = params["Blue Vertices"].toInt();
        int redVertices = params["Red Vertices"].toInt();
        bool complete = params["Complete"].toBool();
        const auto graph = std::make_shared<Bigraph>();
        graph->generate(blueVertices, redVertices);
        graph->constructE(complete);

        auto drawableGraph = std::make_shared<DrawableBigraph>();
        drawableGraph->linkGraph(graph);

        auto *graphWindow = new BigraphWindow(this, drawableGraph, graph);
        graphWindow->setAttribute(Qt::WA_DeleteOnClose);
        graphWindow->show();
        this->hide();

    } else if (graphType == "Comparability") {
        int vertices = params["Vertices"].toInt();
        int dimensions = params["Dimensions"].toInt();
        const auto graph = std::make_shared<ComparabilityGraph>();
        graph->generate(vertices, dimensions, vertices * 100);
        graph->constructE(true);

        auto drawableGraph = std::make_shared<DrawableComparabilityGraph>();
        drawableGraph->linkGraph(graph);

        auto *graphWindow = new ComparabilityGraphWindow(this, drawableGraph, graph);
        graphWindow->setAttribute(Qt::WA_DeleteOnClose);
        graphWindow->show();
        this->hide();

    } else if (graphType == "Bicomparability") {
        int blueVertices = params["Blue Vertices"].toInt();
        int redVertices = params["Red Vertices"].toInt();
        int dimensions = params["Dimensions"].toInt();
        /*const auto graph = std::make_shared<ComparabilityBigraph>();
        graph->generate(blueVertices, redVertices, dimensions, (blueVertices + redVertices) * 100);
        graph->constructE(true);*/

        std::vector<VertexPointer> V{
            std::make_shared<ColoredEmbeddedVertex>(3, 0, std::vector<unsigned>{146, 394}),
            std::make_shared<ColoredEmbeddedVertex>(1, 0, std::vector<unsigned>{457, 841}),
            std::make_shared<ColoredEmbeddedVertex>(6, 0, std::vector<unsigned>{379, 17}),
            std::make_shared<ColoredEmbeddedVertex>(5, 0, std::vector<unsigned>{95, 467}),
            std::make_shared<ColoredEmbeddedVertex>(2, 0, std::vector<unsigned>{197, 530}),
            std::make_shared<ColoredEmbeddedVertex>(7, 1, std::vector<unsigned>{3, 756}),
            std::make_shared<ColoredEmbeddedVertex>(8, 1, std::vector<unsigned>{426, 705}),
            std::make_shared<ColoredEmbeddedVertex>(0, 1, std::vector<unsigned>{896, 316}),
            std::make_shared<ColoredEmbeddedVertex>(4, 1, std::vector<unsigned>{196, 336}),
            std::make_shared<ColoredEmbeddedVertex>(9, 1, std::vector<unsigned>{831, 118}),
        };
        auto graph = std::make_shared<ComparabilityBigraph>(std::move(V), 5 , 5, 2, 900);
        graph->constructE(true);
        
        /*std::vector<VertexPointer> V{
            std::make_shared<ColoredEmbeddedVertex>(36, 0, std::vector<unsigned>{20, 31}),
            std::make_shared<ColoredEmbeddedVertex>(0, 0, std::vector<unsigned>{5, 5}),
            std::make_shared<ColoredEmbeddedVertex>(34, 0, std::vector<unsigned>{0, 23}),
            std::make_shared<ColoredEmbeddedVertex>(33, 0, std::vector<unsigned>{8, 29}),
            std::make_shared<ColoredEmbeddedVertex>(35, 0, std::vector<unsigned>{33, 21}),
            std::make_shared<ColoredEmbeddedVertex>(37, 0, std::vector<unsigned>{22, 25}),
            std::make_shared<ColoredEmbeddedVertex>(38, 0, std::vector<unsigned>{27, 12}),
            std::make_shared<ColoredEmbeddedVertex>(10, 0, std::vector<unsigned>{25, 14}),
            std::make_shared<ColoredEmbeddedVertex>(1, 0, std::vector<unsigned>{18, 36}),
            std::make_shared<ColoredEmbeddedVertex>(31, 0, std::vector<unsigned>{31, 4}),
            std::make_shared<ColoredEmbeddedVertex>(29, 0, std::vector<unsigned>{39, 28}),
            std::make_shared<ColoredEmbeddedVertex>(6, 0, std::vector<unsigned>{15, 19}),
            std::make_shared<ColoredEmbeddedVertex>(24, 0, std::vector<unsigned>{36, 1}),
            std::make_shared<ColoredEmbeddedVertex>(25, 0, std::vector<unsigned>{9, 24}),
            std::make_shared<ColoredEmbeddedVertex>(19, 0, std::vector<unsigned>{28, 3}),
            std::make_shared<ColoredEmbeddedVertex>(13, 0, std::vector<unsigned>{40, 22}),
            std::make_shared<ColoredEmbeddedVertex>(20, 0, std::vector<unsigned>{17, 26}),
            std::make_shared<ColoredEmbeddedVertex>(28, 0, std::vector<unsigned>{24, 0}),
            std::make_shared<ColoredEmbeddedVertex>(17, 0, std::vector<unsigned>{21, 32}),
            std::make_shared<ColoredEmbeddedVertex>(12, 0, std::vector<unsigned>{30, 39}),
            std::make_shared<ColoredEmbeddedVertex>(14, 1, std::vector<unsigned>{2, 38}),
            std::make_shared<ColoredEmbeddedVertex>(8, 1, std::vector<unsigned>{32, 15}),
            std::make_shared<ColoredEmbeddedVertex>(15, 1, std::vector<unsigned>{26, 40}),
            std::make_shared<ColoredEmbeddedVertex>(26, 1, std::vector<unsigned>{1, 35}),
            std::make_shared<ColoredEmbeddedVertex>(39, 1, std::vector<unsigned>{37, 37}),
            std::make_shared<ColoredEmbeddedVertex>(9, 1, std::vector<unsigned>{10, 16}),
            std::make_shared<ColoredEmbeddedVertex>(7, 1, std::vector<unsigned>{34, 11}),
            std::make_shared<ColoredEmbeddedVertex>(11, 1, std::vector<unsigned>{35, 13}),
            std::make_shared<ColoredEmbeddedVertex>(3, 1, std::vector<unsigned>{6, 2}),
            std::make_shared<ColoredEmbeddedVertex>(16, 1, std::vector<unsigned>{13, 27}),
            std::make_shared<ColoredEmbeddedVertex>(30, 1, std::vector<unsigned>{16, 30}),
            std::make_shared<ColoredEmbeddedVertex>(27, 1, std::vector<unsigned>{38, 6}),
            std::make_shared<ColoredEmbeddedVertex>(18, 1, std::vector<unsigned>{11, 8}),
            std::make_shared<ColoredEmbeddedVertex>(23, 1, std::vector<unsigned>{3, 18}),
            std::make_shared<ColoredEmbeddedVertex>(5, 1, std::vector<unsigned>{19, 17}),
            std::make_shared<ColoredEmbeddedVertex>(32, 1, std::vector<unsigned>{23, 33}),
            std::make_shared<ColoredEmbeddedVertex>(2, 1, std::vector<unsigned>{4, 9}),
            std::make_shared<ColoredEmbeddedVertex>(22, 1, std::vector<unsigned>{29, 10}),
            std::make_shared<ColoredEmbeddedVertex>(21, 1, std::vector<unsigned>{14, 20}),
            std::make_shared<ColoredEmbeddedVertex>(4, 1, std::vector<unsigned>{7, 7}),
        };
        auto graph = std::make_shared<ComparabilityBigraph>(std::move(V), 20, 20, 2, 40);
        graph->constructE(true);*/

        auto drawableGraph = std::make_shared<DrawableComparabilityBigraph>();
        drawableGraph->linkGraph(graph);

        auto *graphWindow = new ComparabilityBigraphWindow(this, drawableGraph, graph);
        graphWindow->setAttribute(Qt::WA_DeleteOnClose);
        graphWindow->show();
        this->hide();

    } else if (graphType == "Terrain Visibility") {
        const int vertices = params["Vertices"].toInt();
        const auto terrain = std::make_shared<Terrain>();
        terrain->generate(vertices, vertices * 100);
        const auto visibilityGraph = std::make_shared<TerrainVisibilityGraph>();
        visibilityGraph->generateFromTerrain(*terrain);
        const auto drawableGraph = std::make_shared<DrawableTerrainVisibilityGraph>();
        drawableGraph->linkGraphs(std::static_pointer_cast<Graph>(visibilityGraph), terrain);
        auto* window = new TerrainWindow(this, drawableGraph, visibilityGraph, terrain);
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->show();
        this->hide();
    }
}