#ifndef __PLOT_HPP__
#define __PLOT_HPP__

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtGui/QPainter>
#include <QtCore/QTimer>

#include <vector>

class Plot : public QWidget {
 public:
  Plot(int timeStep=16);
  
  void setRobotPosition(int x, int y) { robot_position = transformPoint(x, y); }
  void setBallPosition(int x, int y) { ball_position = transformPoint(x, y); }
  void setEnemyPosition(std::vector<std::pair<int, int>>& position) {
    for (auto item : position) {
      enemy_position.append(transformPoint(item.first, item.second));
    }
  }
  void setPathPosition(std::vector<std::pair<int, int>>& path) {
    for (auto item : path) {
      data_path.append(transformPoint(item.first, item.second));
    }
  }
  void setBezierPath(std::vector<std::pair<int, int>>& path) {
    for (auto item : path) {
      bezier_path.append(transformPoint(item.first, item.second));
    }
  }
  void setRobotDirection(double dir) { robot_direction = dir; }
  void setAstarPoint(std::vector<std::pair<int, int>>& points) {
    for (auto item : points) {
      astar_point.append(transformPoint(item.first, item.second));
    }
  }
  void setCollision(std::vector<std::pair<int, int>>& points) {
    for (auto item : points) {
      collision.append(transformPoint(item.first, item.second));
    }
  }
  void setTarget(int x, int y) { target = transformPoint(x, y); }

  QPointF transformPoint(int x, int y) { return QPointF(x+470, 320-y); }

 protected:
  void paintEvent(QPaintEvent*) override;
  void updatePlot();

 private:
  QTimer* timer;
  QPointF robot_position, ball_position, target;
  QVector<QPointF> data_path, enemy_position, astar_point, collision, bezier_path;
  double robot_direction;
};

#endif