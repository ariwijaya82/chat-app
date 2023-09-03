#ifndef __PLOT_HPP__
#define __PLOT_HPP__

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtGui/QPainter>
#include <QtCore/QTimer>

#include <vector>
#include <iostream>

using namespace std;

class Monitoring : public QWidget {
 public:
  Monitoring(int timeStep=16);

  void setRobotPosition(pair<int, int>);
  void setBallPosition(pair<int, int>);
  void setEnemyPosition(vector<pair<int, int>>);
  void setCollision(vector<pair<int, int>>);
  void setPath(vector<pair<int, int>>);
  void setBezierPath(vector<pair<int, int>>);

  void setRobotDirection(double);
  void setTarget(pair<int, int>);


 protected:
  void paintEvent(QPaintEvent*) override;
  void updateDisplay();

  QPointF transformPoint(pair<int, int>);

 private:
  QTimer* timer;
  QPointF robot, ball, target;
  QVector<QPointF> path, enemy, imaginary_point, collision, bezier_path;
  double direction;
};

#endif