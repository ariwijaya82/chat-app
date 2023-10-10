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
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>

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
    Monitor();
    ~Monitor();

    void setAStarPath(vector<Vec>);
    void setBezierPath(vector<Vec>);
    void addFollowingPath(Vec);

    void setRobotPosition(Vec);
    void setTarget(Vec);
    void setRobotDirection(double);

    void setNodeVisited(int);
    void setAStarLength(double);
    void setBezierLength(double);
    void setTimeFollow(double);
    void setFollowingError(double);

    void setDisableButton(bool);

  protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void generatePath();
    void clearPath();
    void saveData();

    void updateDisplay();
    QPointF transformPoint(Vec);

  private:
    void updateData();

    GlobalData *global;

    QPointF robot, ball, target;
    QVector<QPointF> enemies, astar_path, bezier_path, visited_node, following_path;
    QVector<QVector<QPointF>> obstacles, obstacles_visible;
    double direction = 0;

    QTimer* timer;
    QPushButton *pushButton[3];
    QLabel *label[10];
    QButtonGroup *buttonGroup[2];
    QHBoxLayout *hBoxLayout;
    QVBoxLayout *vBoxLayout[2];
    QRadioButton *radioButton[9];
    QSpinBox *spinBox[3];
    
    PathGenerator* generator;

    bool clicked = false, isPathGenerated = false;
    int enemy_index;
    QPointF offset;

    int number_of_visited_node = 0;
    double astar_length = 0, bezier_length = 0, time_following = 0; 
};

#endif