#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtGui/QPainter>
#include <QtCore/QTimer>

class RealTimePlot : public QWidget {
 public:
  RealTimePlot(QWidget* parent = nullptr);
 protected:
  void paintEvent(QPaintEvent*) override;
  void updatePlot();
 private:
  QTimer* timer;
  double currentTime;
  QVector<QPointF> dataPoints;
};
