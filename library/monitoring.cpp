#include "monitoring.hpp"

Monitoring::Monitoring(int timeStep){
    setFixedSize(940, 640);
    setWindowTitle("Robot State");

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Monitoring::updateDisplay);
    timer->start(timeStep);

    ifstream global_file("config/global.json");
    json global = json::parse(global_file);
    HEIGHT = global["screen_height"].template get<double>();
    WIDTH = global["screen_width"].template get<double>();
    PADDING = global["screen_padding"].template get<double>();
    NODE_DISTANCE = global["node_distance"].template get<double>();
    ENEMY_RADIUS = global["enemy_radius"].template get<double>();
}

QPointF Monitoring::transformPoint(pair<double, double> point) {
    return QPointF{point.first + PADDING, HEIGHT + PADDING - point.second};
}

void Monitoring::setRobotPosition(pair<double, double> point) {
    robot = transformPoint(point);
}

void Monitoring::setBallPosition(pair<double, double> point) {
    ball = transformPoint(point);
}

void Monitoring::setEnemyPosition(vector<pair<double, double>> points) {
    for (auto item : points) {
        enemy.push_back(transformPoint(item));
    }
}

void Monitoring::setCollision(vector<pair<double, double>> points) {
    for (auto item : points) {
        collision.push_back(transformPoint(item));
    }
}

void Monitoring::setPath(vector<pair<double, double>> points) {
    for (auto item : points) {
        path.push_back(transformPoint(item));
    }
}

void Monitoring::setBezierPath(vector<pair<double, double>> points) {
    for (auto item : points) {
        bezier_path.push_back(transformPoint(item));
    }
}

void Monitoring::setRobotDirection(double dir) {
    direction = dir;
}

void Monitoring::setTarget(pair<double, double> point) {
    target = transformPoint(point);
}

void Monitoring::updateDisplay() {
    update();
}

void Monitoring::paintEvent(QPaintEvent*) {
    QPainter painter(this);

    // draw field
    painter.fillRect(rect(), Qt::green);
    painter.setPen(Qt::white);
    painter.drawLine(QPointF{20,20}, QPointF{920,20});
    painter.drawLine(QPointF{20,20}, QPointF{20,620});
    painter.drawLine(QPointF{920,620}, QPointF{920,20});
    painter.drawLine(QPointF{920,620}, QPointF{20,620});

    painter.drawLine(QPointF{20,70}, QPointF{220,70});
    painter.drawLine(QPointF{220,70}, QPointF{220,570});
    painter.drawLine(QPointF{20,570}, QPointF{220,570});
    painter.drawLine(QPointF{20,170}, QPointF{120,170});
    painter.drawLine(QPointF{120,170}, QPointF{120,470});
    painter.drawLine(QPointF{20,470}, QPointF{120,470});
    
    painter.drawLine(QPointF{920,70}, QPointF{720,70});
    painter.drawLine(QPointF{720,70}, QPointF{720,570});
    painter.drawLine(QPointF{920,570}, QPointF{720,570});
    painter.drawLine(QPointF{920,170}, QPointF{820,170});
    painter.drawLine(QPointF{820,170}, QPointF{820,470});
    painter.drawLine(QPointF{920,470}, QPointF{820,470});

    painter.drawLine(QPointF{470,20}, QPointF{470,620});
    painter.drawEllipse(QPointF{470, 320}, 100, 100);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawEllipse(robot, 10, 10);
    painter.setBrush(Qt::cyan);
    painter.drawEllipse(ball, 10, 10);

    painter.setPen(Qt::black);
    painter.drawLine(robot.x(), robot.y(),
    robot.x() + 40 * cos(direction),
    robot.y() + 40 * sin(direction));

    painter.setPen(Qt::red);
    for (int i = 0; i < enemy.size(); i++) {
        painter.setBrush(Qt::red);
        painter.drawEllipse(enemy[i], 10, 10);
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(enemy[i], ENEMY_RADIUS, ENEMY_RADIUS);
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::yellow);
    for (int x = NODE_DISTANCE; x < WIDTH; x += NODE_DISTANCE) {
      for (int y = NODE_DISTANCE; y < HEIGHT; y += NODE_DISTANCE) {
        painter.drawEllipse(transformPoint(pair<double, double>{x, y}), 2, 2);
      }
    }
    painter.setBrush(Qt::red);
    for (int i = 0; i < collision.size(); i++) {
        painter.drawEllipse(collision[i], 2, 2);
    }
    painter.setBrush(Qt::gray);
    painter.setPen(Qt::gray);
    for (int i = 0; i < path.size(); i++) {
        painter.drawEllipse(path[i], 2, 2);
        if (i != 0) {
            painter.drawLine(path[i], path[i-1]);
        }
    }
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::magenta);
    for (int i = 1; i < bezier_path.size(); i++) {
        painter.drawLine(bezier_path[i], bezier_path[i-1]);
    }

    painter.setPen(Qt::cyan);
    painter.drawLine(robot, target);
}
