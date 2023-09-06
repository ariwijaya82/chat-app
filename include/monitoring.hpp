#ifndef __PLOT_HPP__
#define __PLOT_HPP__

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtGui/QPainter>
#include <QtCore/QTimer>

#include <vector>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace std;
using nlohmann::json;

class Monitoring : public QWidget {
 public:
  Monitoring(int timeStep=16);

  void setRobotPosition(pair<double, double>);
  void setBallPosition(pair<double, double>);
  void setEnemyPosition(vector<pair<double, double>>);
  void setCollision(vector<pair<double, double>>);
  void setPath(vector<pair<double, double>>);
  void setBezierPath(vector<pair<double, double>>);

  void setRobotDirection(double);
  void setTarget(pair<double, double>);

 protected:
  void paintEvent(QPaintEvent*) override;
  void updateDisplay();

  QPointF transformPoint(pair<double, double>);

 private:
  QTimer* timer;
  QPointF robot, ball, target;
  QVector<QPointF> path, enemy, imaginary_point, collision, bezier_path;
  double direction;

  double HEIGHT, WIDTH, PADDING, NODE_DISTANCE, ENEMY_RADIUS;
};

#endif