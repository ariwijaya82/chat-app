#ifndef PLOT_HPP
#define PLOT_HPP

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtGui/QPainter>
#include <QtCore/QTimer>

class RealTimePlot : public QWidget {
 public:
  RealTimePlot(int timeStep=0, QWidget* parent=nullptr);
  void addDataPosition(int, int);
  void addDataPath(int, int);
  void setDirection(double dirRad) { directionRadian = dirRad; }
  void setTargetDir(double dirRad) { targetDir = dirRad; }
  void setTarget(int x, int y) { target = QPointF(x, y); }
  QPointF transformPoint(int x, int y) { return QPointF(x+470, 320-y); }

 protected:
  void paintEvent(QPaintEvent*) override;
  void updatePlot();

 private:
  QTimer* timer;
  QVector<QPointF> dataPosition;
  QVector<QPointF> dataPath;
  double directionRadian;
  double targetDir;
  QPointF target;
};

#endif