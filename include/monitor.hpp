#ifndef __MONITOR_HPP__
#define __MONITOR_HPP__

#include <QtCore/QTimer>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
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
  Monitor(int timeStep=16, string type="other");
  ~Monitor();

  void setAStarPath(vector<Vec>);
  void setBezierPath(vector<Vec>);

  void setRobotPosition(Vec);
  void setTarget(Vec);
  void setRobotDirection(double);

 protected:
  void paintEvent(QPaintEvent*) override;
  void mousePressEvent(QMouseEvent*) override;
  void mouseMoveEvent(QMouseEvent*) override;
  void mouseReleaseEvent(QMouseEvent*) override;
  void generatePath();
  void clearPath();
  void updateWorldFile();

  void updateDisplay();
  QPointF transformPoint(Vec);

 private:
  QTimer* timer;
  QPushButton *generateButton, *clearButton, *updateButton;
  QPointF robot, ball, target;
  QVector<QPointF> enemies, astar_path, bezier_path;
  QVector<QVector<QPointF>> obstacles;
  double direction = 0;

  Constant* constant;
  double HEIGHT, WIDTH, PADDING, NODE_DISTANCE, ENEMY_RADIUS;
  bool clicked = false;
  int enemy_index;
  QPointF offset;

  PathGenerator* generator;
  string monitor_type;
};

#endif