#ifndef __MONITOR_HPP__
#define __MONITOR_HPP__

#include <QtCore/QTimer>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSlider>
#include <QtWidgets/QCheckBox>
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtWebSockets/QWebSocket>

#include <vector>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "utils.hpp"
#include "path_generator.hpp"

using namespace std;
using nlohmann::json;

class Monitor : public QWidget {
  public:
    Monitor(GlobalData*);
    ~Monitor() {};

    void updateData();
    void updateDisplay();

  protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    
  private:
    void displayPanel();
    
    void handleModeChange(int);
    
    void handleLeftButton();
    void handleRightButton();
    void handleBottomButton();

    void handleWebsocketMessage(const QString&);

    QPointF transformPoint(Vec);
    void generatePath();

    GlobalData *global;
    PathGenerator* generator;

    QPointF robot, ball, target;
    QVector<QPointF> enemies, astar_path, bezier_path, visited_node, following_path;
    QVector<QVector<QPointF>> obstacles, obstacles_visible;
    
    double direction[6];
    bool connected[6];
    int mode = 0;
    bool isGenerate = false;
    bool isStatic = false;
    bool isStart = false;

    QWebSocket *robotSocket[6];
    QPushButton *leftButton,
                *rightButton,
                *bottomButton;
    QLabel *pathLabel,
           *heuristicLabel,
           *nodeLabel,
           *radiusLabel,
           *curvatureLabel,
           *bezierLabel;
    QComboBox *pathList,
              *heuristicList;
    QSpinBox *nodeNumber,
             *radiusNumber,
             *curvatureNumber;
    QSlider *bezierSlider;
    QCheckBox *staticCheckbox;


    
    bool clicked = false;
    int enemy_index;
    QPointF offset;
};

#endif