#ifndef MEMOIRE_GRAPHWINDOW_HPP
#define MEMOIRE_GRAPHWINDOW_HPP

#include <QCheckBox>
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>

#include "DrawableGraph.hpp"

class GraphWindow : public QMainWindow {
    Q_OBJECT

public:
    GraphWindow(QMainWindow *creator, const std::shared_ptr<DrawableComparabilityBigraph>& drawable, const std::shared_ptr<ComparabilityBigraph>& graph);
    ~GraphWindow() override;



private slots:
    void onComputeBicliqueCoverClicked();
    void onOptimizeToggled(bool checked);
    void onComputeBFSClicked();

private:
    void setupUI();
    void appendOutput(const QString &text) const;
    void clearOutput();

    QWidget *canvasWidget;
    QTextEdit *outputTextEdit;
    QPushButton *computeBicliqueCoverBtn;
    QCheckBox *showStepsCheckBox;
    QPushButton *showStepsBtn;
    QPushButton *computeBFSBtn;

    bool optimize;

    QMainWindow* creator;
    std::shared_ptr<DrawableComparabilityBigraph> drawable;
    std::shared_ptr<ComparabilityBigraph> graph;
};

#endif // MEMOIRE_GRAPHWINDOW_HPP
