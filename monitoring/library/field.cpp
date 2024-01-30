#include "field.hpp"
#include "utils.hpp"

Field::Field(QWidget* parent) {
    setParent(parent);
    param par = loadParameter();
    setFixedSize(par.screen_width + 2*par.screen_padding,
        par.screen_height + 2*par.screen_padding);

    mode = 0;

    node_distance = par.node_distance;
    robot_radius = par.robot_radius;
    screen_width = par.screen_width;
    screen_height = par.screen_height;

    position pos = loadPosition(par.path_number);
    ball = convertCartesianToQt(pos.ball);
    robot = convertCartesianToQt(pos.robot);
    direction.push_back(0.0);
    for (auto enemy : pos.enemies) {
        enemies.push_back(convertCartesianToQt(enemy));
        direction.push_back(M_PI);
    }
}


void Field::paintEvent(QPaintEvent* paint) {
    QPainter painter(this);
    painter.fillRect(rect(), QColor(89,192,52));
    switch (mode) {
        case 0: // mode Simulate 
        {
            painter.setPen(QPen(Qt::white, 3));
            painter.drawRect(20, 20, 900, 600);
            painter.drawRect(20, 70, 200, 500);
            painter.drawRect(20, 170, 100, 300);
            painter.drawRect(720, 70, 200, 500);
            painter.drawRect(820, 170, 100, 300);
            painter.drawEllipse(QPoint(470, 320), 100, 100);
            painter.drawLine(470, 20, 470, 620);

            painter.setPen(Qt::yellow);
            painter.setBrush(Qt::yellow);
            for (int x = node_distance; x < screen_width; x+=node_distance) {
                for (int y = node_distance; y < screen_height; y+=node_distance) {
                    painter.drawEllipse(convertCartesianToQt(Point((double)x, (double)y)), 2, 2);
                }
            }

            painter.setPen(Qt::black);
            painter.setBrush(Qt::black);
            painter.drawEllipse(robot, 8, 8);
            painter.setBrush(Qt::NoBrush);
            painter.drawEllipse(robot, robot_radius/2, robot_radius/2);
            painter.drawLine(robot, robot + QPointF(cos(direction[0]) * robot_radius / 2, -sin(direction[0]) *robot_radius / 2));

            painter.setPen(Qt::blue);
            painter.setBrush(Qt::blue);
            painter.drawEllipse(ball, 8, 8);

            int index = 1;
            for (auto enemy : enemies) {
                painter.setPen(Qt::red);
                painter.setBrush(Qt::red);
                painter.drawEllipse(enemy, 8, 8);
                painter.drawLine(enemy, enemy + QPointF(cos(direction[index]) * robot_radius / 2, -sin(direction[index]) *robot_radius / 2));
                painter.setBrush(Qt::NoBrush);
                painter.drawEllipse(enemy, robot_radius/2, robot_radius/2);
                painter.setPen(QPen(Qt::red, 3, Qt::DashLine));
                painter.drawEllipse(enemy, robot_radius, robot_radius);
                index++;
            }

            break;
        }
        case 1: // mode A-Star
        {
            break;
        }
        case 2: // mode Bezier
        {
            break;
        }
    }
}

