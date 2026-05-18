#ifndef MEMOIRE_GRAPHWINDOW_HPP
#define MEMOIRE_GRAPHWINDOW_HPP

#include <qboxlayout.h>
#include <QCheckBox>
#include <qlabel.h>
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>

#include "../graphicsItems/DrawableGraph.hpp"

class GraphWindow : public QMainWindow {
    Q_OBJECT
public:
    GraphWindow(QMainWindow *creator, const std::shared_ptr<DrawableGraph>& drawable, const std::shared_ptr<Graph>& graph);
    ~GraphWindow() override;
protected slots:
    void onReShowMainWindow();

protected:
    void setupUI();
    void setupCanvas(QVBoxLayout *main_layout);
    void setupConsole(QVBoxLayout *main_layout);
    void setupButtonsArea(QVBoxLayout *main_layout);
    virtual void setupButtons(QGridLayout *grid_layout);
    void appendOutput(const QString &text) const;
    void clearOutput();

    QWidget *canvas_widget{};
    QTextEdit *output_text_edit{};
    QPushButton *main_window_visibility_manager{};

    QMainWindow* creator;
    bool is_creator_displayed; //todo static and update on close, should be in another class
    QString title;
    QString window_title;
    QGraphicsScene* graphics_scene = nullptr;
    QGraphicsView* graphics_view = nullptr;
    std::shared_ptr<DrawableGraph> drawable;
    std::shared_ptr<Graph> graph;

    void resizeEvent(QResizeEvent* event) override;
    void displayGraph(const std::shared_ptr<Graph>& graph);
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

    QPushButton *compute_biclique_cover_btn{};
    QCheckBox *optimize_size_button{};
    QPushButton *showStepsBtn{};
    QPushButton *computeBFSBtn{};

    bool optimize = false;
};



class TerrainWindow : public GraphWindow {
    Q_OBJECT
public:
    TerrainWindow(QMainWindow *creator, const std::shared_ptr<DrawableTerrainVisibilityGraph>& drawable,
        const std::shared_ptr<TerrainVisibilityGraph>& visibility_graph, const std::shared_ptr<Terrain>& terrain);
    ~TerrainWindow() override;

protected slots:
    void onToggleViewClicked();
    void onComputeBicliqueCoverClicked();
    void onOptimizeToggled(bool checked);

protected:
    void setupButtons(QGridLayout *grid_layout) override;
    void setupToggleView(QGridLayout *grid_layout);
    void setupComputeBicliqueCover(QGridLayout *grid_layout);
    void updateView();

    std::shared_ptr<DrawableTerrainVisibilityGraph> drawable_visibility_graph;
    std::shared_ptr<TerrainVisibilityGraph> visibility_graph;
    std::shared_ptr<Terrain> terrain;

    QPushButton *toggle_view_btn{};
    QPushButton *compute_biclique_cover_btn{};
    QCheckBox *optimize_size_button{};

    bool showing_terrain = false;
    bool optimize = false;
};

#endif // MEMOIRE_GRAPHWINDOW_HPP
