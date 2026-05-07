#ifndef MEMOIRE_MAINWINDOW_HPP
#define MEMOIRE_MAINWINDOW_HPP

#include <QMainWindow>
#include <QComboBox>
#include <QGridLayout>
#include <QMap>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onGraphTypeChanged(int index);
    void onGenerateClicked();

private:
    void setupUI();

    QPushButton* instantiateRandomButton();
    QSpinBox* instantiateSpinBox(const QString &type);
    std::pair<QPushButton*, QSpinBox*> instantiateIntegerBox(const QString &type, const QString &label);
    QPushButton* instantiateToggleButton(const QString &label);
    QWidget* setupCentralWidget();
    QWidget* setupHeader();
    QWidget* setupFooter();
    QWidget* setupBody();
    QComboBox* setupGraphTypeComboBox();

    void populateGraphOptions(const QString &graphType);
    void clearOptions();

    QMap<QString, QVariant> getFormParameters() const;

    QComboBox *graphTypeCombo;
    QWidget *optionsWidget;
    QGridLayout *optionsLayout;
    QWidget *contentWidget;
    QPushButton *generateBtn;

    QMap<QString, QSpinBox*> spinBoxes;
    QMap<QString, QPushButton*> toggleButtons;
    QMap<QString, QPushButton*> randomButtons;

    struct GraphOptions {
        QStringList labels;
        QStringList types; // "vertices", "dimensions", "toggle"
    };

    QMap<QString, GraphOptions> graphConfigs = {
        {"Simple", {
                {"Vertices", "Complete"},
                {"vertices", "toggle"}}},
            {"Bipartite", {
                {"Blue Vertices", "Red Vertices", "Complete"},
                {"vertices", "vertices", "toggle"}}},
            {"Comparability", {
                {"Vertices", "Dimensions"},
                {"vertices", "dimensions"}}},
            {"Bicomparability", {
                {"Blue Vertices", "Red Vertices", "Dimensions"},
                {"vertices", "vertices", "dimensions"}}}
    };

};

#endif // MEMOIRE_MAINWINDOW_HPP