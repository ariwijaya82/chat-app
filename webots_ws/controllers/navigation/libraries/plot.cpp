#include "plot.hpp"

Plot::Plot(int timeStep){
    setFixedSize(940, 640);
    setWindowTitle("Robot State Plot");

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Plot::updatePlot);
    timer->start(timeStep);
}

void Plot::updatePlot() {
    update();
}

void Plot::paintEvent(QPaintEvent*) {
    QPainter painter(this);

    painter.fillRect(rect(), Qt::green);
    painter.setPen(Qt::white);
    painter.drawLine(QPointF{20,20}, QPointF{920,20});
    painter.drawLine(QPointF{20,20}, QPointF{20,620});
    painter.drawLine(QPointF{920,620}, QPointF{920,20});
    painter.drawLine(QPointF{920,620}, QPointF{20,620});
    painter.drawLine(QPointF{470,20}, QPointF{470,620});
    painter.drawLine(QPointF{20,120}, QPointF{170,120});
    painter.drawLine(QPointF{20,520}, QPointF{170,520});
    painter.drawLine(QPointF{170,120}, QPointF{170,520});
    painter.drawLine(QPointF{920,120}, QPointF{770,120});
    painter.drawLine(QPointF{920,520}, QPointF{770,520});
    painter.drawLine(QPointF{770,120}, QPointF{770,520});

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawEllipse(robot_position, 10, 10);
    painter.setBrush(Qt::cyan);
    painter.drawEllipse(ball_position, 10, 10);

    painter.setPen(Qt::black);
    painter.drawLine(robot_position.x(), robot_position.y(),
    robot_position.x() + 40 * cos(robot_direction),
    robot_position.y() + 40 * sin(robot_direction));

    for (int i = 0; i < enemy_position.size(); i++) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::red);
        painter.drawEllipse(enemy_position[i], 10, 10);
        painter.setBrush(Qt::NoBrush);
        painter.setPen(Qt::red);
        painter.drawEllipse(enemy_position[i], 50, 50);
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::yellow);
    for (int i = 0; i < astar_point.size(); i++) {
        painter.drawEllipse(astar_point[i], 2, 2);
    }
    painter.setBrush(Qt::red);
    for (int i = 0; i < collision.size(); i++) {
        painter.drawEllipse(collision[i], 2, 2);
    }
    painter.setBrush(Qt::gray);
    for (int i = 0; i < data_path.size(); i++) {
        painter.drawEllipse(data_path[i], 2, 2);
    }
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::magenta);
    for (int i = 1; i < bezier_path.size(); i++) {
        painter.drawLine(bezier_path[i], bezier_path[i-1]);
    }

    painter.setPen(Qt::cyan);
    painter.drawLine(robot_position, target);
}
