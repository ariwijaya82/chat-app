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
#include <QtWidgets/QMessageBox>

#include "render_area.hpp"

class Panel : public QWidget {
  public:
    Panel(GlobalData* global);
    void setMode(int); 

  private:
    RenderArea *renderArea;
    GlobalData *global;

    QPushButton *leftButton;
    QPushButton *rightButton;
    QPushButton *connectButton;
    QPushButton *startButton;
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

    void handleLeftButton();
    void handleRightButton();
    void handleConnectButton();
    void handleStartButton();
};

#endif