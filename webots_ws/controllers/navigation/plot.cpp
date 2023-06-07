#include "plot.hpp"

RealTimePlot::RealTimePlot(int timeStep, QWidget* parent) : QWidget(parent) {
    setFixedSize(940, 640);
    setWindowTitle("Robot State Plot");

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &RealTimePlot::updatePlot);
    timer->start(timeStep);
}

void RealTimePlot::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);

    QRect field(20, 20, 900, 600);
    painter.fillRect(field, Qt::green);

    painter.setPen(Qt::red);
    for (int i = 0; i < dataPosition.size(); i++) {
        painter.drawEllipse(dataPosition[i], 2, 2);
    }
    if (!target.isNull())
        painter.drawLine(dataPosition.back(), transformPoint(target.x(), target.y()));

    painter.setPen(Qt::yellow);
    for (int i = 1; i < dataPath.size(); i++) {
        painter.drawLine(dataPath[i], dataPath[i-1]);
    }

    painter.setPen(Qt::blue);
    if (dataPosition.size() == 1) {
        int radius = 40;
        painter.drawLine(
            dataPosition[0].x(),
            dataPosition[0].y(),
            dataPosition[0].x() + radius*cos(directionRadian),
            dataPosition[0].y() + radius*sin(directionRadian));
        // painter.setPen(Qt::yellow);
        // painter.drawLine(
        //     dataPosition[0].x(),
        //     dataPosition[0].y(),
        //     dataPosition[0].x() + radius*cos(targetDir),
        //     dataPosition[0].y() + radius*sin(targetDir));
    }
}

void RealTimePlot::updatePlot() {
    update();
}

void RealTimePlot::addDataPosition(int x, int y) {
    QPointF point = transformPoint(x, y);
    dataPosition.append(point);
    if (dataPosition.size() > 1) dataPosition.removeFirst();
}

void RealTimePlot::addDataPath(int x, int y) {
    QPointF point = transformPoint(x, y);
    dataPath.append(point);
}
