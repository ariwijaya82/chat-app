#ifndef __PANEL_HPP__
#define __PANEL_HPP__

#include <QtWidgets/QWidget>

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QFormLayout>

#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QSlider>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QLabel>

#include "render_area.hpp"

class Panel : public QWidget {
  public:
    Panel(GlobalData* global);
    void setMode(int); 

  private:
    RenderArea *render_area;
    GlobalData *global;

    QPushButton *leftButton;
    QPushButton *middleButton;
    QPushButton *rightButton;
    QCheckBox *staticCheck;
    QSlider *bezierSlider;
    QComboBox *pathCombo;
    QComboBox *heuristicCombo;
    QSpinBox *nodeSpin;
    QSpinBox *radiusSpin;
    QSpinBox *bezierSpin;
    QLabel *pathLabel;
    QLabel *heuristicLabel;
    QLabel *nodeLabel;
    QLabel *radiusLabel;
    QLabel *bezierSpinLabel;
    QLabel *bezierSliderLabel;

    int mode;
    bool isGenerate;
    bool isStart;
    bool isConnected;
    bool isStatic;

    void handleLeftButton();
    void handleMiddleButton();
    void handleRightButton();

    void handlePathCombo(int);
    void handleHeuristicCombo(int);
    void handleNodeSpin(int);
    void handleRadiusSpin(int);
    void handleBezierSpin(int);
};

#endif