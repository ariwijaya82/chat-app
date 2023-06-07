#include "plot.hpp"

RealTimePlot::RealTimePlot(QWidget* parent) : QWidget(parent) {
    setFixedSize(940, 640);
    setWindowTitle("Robot State Plot");

    dataPoints.reserve(10);
}

void RealTimePlot::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);

    QRect field(20, 20, 900, 600);
    painter.fillRect(field, Qt::green);

    painter.setPen(Qt::black);
    painter.drawPoint(470, 320);

    painter.setPen(Qt::blue);
    for (int i = 0; i < dataPoints.size(); i++) {
        painter.drawPoint(dataPoints[i]);
    }
}

void RealTimePlot::updatePlot(double x, double y) {
    QPointF point(x, y);
    dataPoints.append(point);

    if (dataPoints.size() > 10) dataPoints.removeFirst();
    update();
}