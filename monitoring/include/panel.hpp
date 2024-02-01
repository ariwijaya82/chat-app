#ifndef __PANEL_HPP__
#define __PANEL_HPP__

#include <QtCore/QTimer>
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
#include <QtWebSockets/QWebSocket>

#include <nlohmann/json.hpp>

#include "render_area.hpp"

using nlohmann::json;

class Panel : public QWidget {
  public:
    Panel(GlobalData* global);
    void setMode(int);
    void setView(int, bool);

  private:
    RenderArea *renderArea;
    GlobalData *global;
    
    QTimer *timer;
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
    QWebSocket *robotSocket[6];

    void handleLeftButton();
    void handleRightButton();
    void handleConnectButton();
    void handleStartButton();
    
    void handleTimer();
    void handleSocketMessage(int, string);
};

#endif