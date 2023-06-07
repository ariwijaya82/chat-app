#ifndef PLOT_HPP
#define PLOT_HPP

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtGui/QPainter>
#include <QtCore/QTimer>

class RealTimePlot : public QWidget {
 public:
  RealTimePlot(QWidget* parent=nullptr);
  void updatePlot(double, double);

 protected:
  void paintEvent(QPaintEvent*) override;

 private:
  QVector<QPointF> dataPoints;
};

#endif