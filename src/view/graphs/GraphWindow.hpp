#ifndef MEMOIRE_GRAPHWINDOW_HPP
#define MEMOIRE_GRAPHWINDOW_HPP

#include <qboxlayout.h>
#include <QCheckBox>
#include <qlabel.h>
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>

#include "../widgets/DrawableGraph.hpp"

class GraphWindow : public QMainWindow {
    Q_OBJECT
public:
    GraphWindow(QMainWindow *creator, const std::shared_ptr<DrawableGraph>& drawable, const std::shared_ptr<Graph>& graph);
    ~GraphWindow() override;
protected slots:
    void onReShowMainWindow();

protected:
    void setupUI();
    void setupCanvas(QVBoxLayout *mainLayout);
    void setupConsole(QVBoxLayout *mainLayout);
    void setupButtonsArea(QVBoxLayout *mainLayout);
    virtual void setupButtons(QGridLayout *grid_layout);
    void appendOutput(const QString &text) const;
    void clearOutput();

    QWidget *canvasWidget{};
    QTextEdit *outputTextEdit{};
    QPushButton *mainWindowVisibilityManager{};

    QMainWindow* creator;
    bool is_creator_displayed; //todo static and update on close, should be in another class
    QString title;
    QString window_title;
    std::shared_ptr<DrawableGraph> drawable;
    std::shared_ptr<Graph> graph;
};



class BigraphWindow : public GraphWindow {
    Q_OBJECT
public:
    BigraphWindow(QMainWindow *creator, const std::shared_ptr<DrawableBigraph>& drawable, const std::shared_ptr<Bigraph>& graph);
    ~BigraphWindow() override;
};


class ComparabilityGraphWindow : public GraphWindow {
    Q_OBJECT
public:
    ComparabilityGraphWindow(QMainWindow *creator, const std::shared_ptr<DrawableComparabilityGraph>& drawable, const std::shared_ptr<ComparabilityGraph>& graph);
    ~ComparabilityGraphWindow() override;
};


class ComparabilityBigraphWindow : public GraphWindow {
    Q_OBJECT
public:
    ComparabilityBigraphWindow(QMainWindow *creator, const std::shared_ptr<DrawableComparabilityBigraph>& drawable, const std::shared_ptr<ComparabilityBigraph>& graph);
    ~ComparabilityBigraphWindow() override;

protected slots:
    void onComputeBicliqueCoverClicked();
    void onOptimizeToggled(bool checked);
    void onComputeBFSClicked();
    void onShowStepsClicked();

protected:
    void setupButtons(QGridLayout *grid_layout) override;
    void setupComputeBicliqueCover(QGridLayout *grid_layout);
    void setupShowSteps(QGridLayout *grid_layout);
    void setupComputeBFS(QGridLayout *grid_layout);

    QPushButton *computeBicliqueCoverBtn{};
    QCheckBox *showStepsCheckBox{};
    QPushButton *showStepsBtn{};
    QPushButton *computeBFSBtn{};

    bool optimize{};
};

#endif // MEMOIRE_GRAPHWINDOW_HPP
