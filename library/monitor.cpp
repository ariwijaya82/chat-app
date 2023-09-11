#include "monitor.hpp"

Monitor::Monitor(int timeStep, string type){
    setFixedSize(940, 640);
    setWindowTitle("Robot State");

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Monitor::updateDisplay);
    timer->start(timeStep);

    monitor_type = type;
    if (monitor_type == "other") {
        generateButton = new QPushButton("generate", this);
        generateButton->setGeometry(300, 0, 100, 30);

        clearButton = new QPushButton("clear", this);
        clearButton->setGeometry(420, 0, 100, 30);

        updateButton = new QPushButton("update", this);
        updateButton->setGeometry(540, 0, 100, 30);

        connect(generateButton, &QPushButton::clicked, [&]() {
            generatePath();
        });
        connect(clearButton, &QPushButton::clicked, [&]() {
            clearPath();
        });
        connect(updateButton, &QPushButton::clicked, [&]() {
            try {
                updateWorldFile();
                QMessageBox::information(this, "Success", "Success update world file");
            } catch(...) {
                QMessageBox::information(this, "Error", "Failed update world file");
            }
        });

        generator = new PathGenerator();
    }

    constant = new Constant();
    HEIGHT = constant->height;
    WIDTH = constant->width;
    PADDING = constant->padding;
    NODE_DISTANCE = constant->distance;
    ENEMY_RADIUS = constant->radius;

    robot = target = transformPoint(constant->robot);
    ball = transformPoint(constant->ball);
    for (auto &enemy : constant->enemies) {
        enemies.push_back(transformPoint(enemy));
    }
    for (auto &group_obstacles : constant->obstacles){
        QVector<QPointF> new_obstacles;
        for (auto &obstacle : group_obstacles) {
            new_obstacles.push_back(transformPoint(obstacle));
        }
        obstacles.push_back(new_obstacles);
    }
}

Monitor::~Monitor() {
    delete timer;
    delete constant;
    delete generator;
    delete generateButton;
    delete clearButton;
    delete updateButton;
}

QPointF Monitor::transformPoint(Vec point) {
    return QPointF{point.x + PADDING, HEIGHT + PADDING - point.y};
};

void Monitor::setAStarPath(vector<Vec> points) {
    astar_path.clear();
    for (auto &point : points) {
        astar_path.push_back(transformPoint(point));
    }
}

void Monitor::setBezierPath(vector<Vec> points) {
    bezier_path.clear();
    for (auto &point : points) {
        bezier_path.push_back(transformPoint(point));
    }
}

void Monitor::setRobotPosition(Vec point) {
    robot = transformPoint(point);
}

void Monitor::setTarget(Vec point) {
    target = transformPoint(point);
}

void Monitor::setRobotDirection(double dir) {
    direction = dir;
}

void Monitor::updateDisplay() {
    update();
}

void Monitor::paintEvent(QPaintEvent*) {
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
    for (int i = 0; i < enemies.size(); i++) {
        painter.setBrush(Qt::red);
        painter.drawEllipse(enemies[i], 10, 10);
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(enemies[i], ENEMY_RADIUS, ENEMY_RADIUS);
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::yellow);
    for (int x = NODE_DISTANCE; x < WIDTH; x += NODE_DISTANCE) {
      for (int y = NODE_DISTANCE; y < HEIGHT; y += NODE_DISTANCE) {
        painter.drawEllipse(transformPoint(Vec(x, y)), 2, 2);
      }
    }
    painter.setBrush(Qt::red);
    for (auto &group_obstacles : obstacles){
        for (auto &obstacle : group_obstacles) {
            painter.drawEllipse(obstacle, 2, 2);
        }
    }
    painter.setBrush(Qt::gray);
    painter.setPen(Qt::gray);
    for (int i = 0; i < astar_path.size(); i++) {
        painter.drawEllipse(astar_path[i], 2, 2);
        if (i != 0) {
            painter.drawLine(astar_path[i], astar_path[i-1]);
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

void Monitor::mousePressEvent(QMouseEvent* event) {
    if (monitor_type == "other") {
        enemy_index = -1;
        if (event->pos().x() >= robot.x() - 10 &&
            event->pos().x() <= robot.x() + 10 &&
            event->pos().y() >= robot.y() - 10 &&
            event->pos().y() <= robot.y() + 10) {
                enemy_index = enemies.size();
                offset = event->pos() - robot;
            }
        else if (event->pos().x() >= ball.x() - 10 &&
            event->pos().x() <= ball.x() + 10 &&
            event->pos().y() >= ball.y() - 10 &&
            event->pos().y() <= ball.y() + 10) {
                enemy_index = enemies.size() + 1;
                offset = event->pos() - ball;
            }
        else {
            for (int i = 0; i < enemies.size(); i++) {
                if (event->pos().x() >= enemies[i].x() - ENEMY_RADIUS &&
                    event->pos().x() <= enemies[i].x() + ENEMY_RADIUS &&
                    event->pos().y() >= enemies[i].y() - ENEMY_RADIUS &&
                    event->pos().y() <= enemies[i].y() + ENEMY_RADIUS) {
                        enemy_index = i;
                        offset = event->pos() - enemies[enemy_index];
                        break;
                    }
            }
        }
        if (event->button() == Qt::LeftButton && enemy_index != -1) {
            clicked = true;
        }
    }
}

void Monitor::mouseMoveEvent(QMouseEvent* event) {
    if (monitor_type == "other" && clicked) {
        if (enemy_index < enemies.size())
            enemies[enemy_index] = event->pos() - offset;
        else if (enemy_index == enemies.size()) {
            robot = event->pos() - offset;
            target = robot;
        } 
        else ball = event->pos() - offset;
    }
}

void Monitor::mouseReleaseEvent(QMouseEvent* event) {
    if (monitor_type == "other" && clicked) {
        clicked = false;
        if (enemy_index < enemies.size()) {
            obstacles[enemy_index].clear();
            QPointF temp_point{
                static_cast<int>((enemies[enemy_index].x() - PADDING) / NODE_DISTANCE) * NODE_DISTANCE + PADDING,
                static_cast<int>((enemies[enemy_index].y() - PADDING) / NODE_DISTANCE) * NODE_DISTANCE + PADDING
            };
            int offset = static_cast<int>(ENEMY_RADIUS / NODE_DISTANCE + 1);

            auto point_in_field = [&](QPointF point) {
                if (
                    point.x() >= PADDING && point.x() <= WIDTH &&
                    point.y() >= PADDING && point.y() <= HEIGHT
                ) return true;
                return false;
            };

            auto len_point = [](QPointF point) {
                return sqrt(point.x()*point.x() + point.y()*point.y());
            };

            for (int i = -offset; i <= offset; i++) {
                for (int j = -offset; j <= offset; j++) {
                    QPointF neighbor = temp_point + QPointF{i * NODE_DISTANCE, j * NODE_DISTANCE};
                    QPointF delta = enemies[enemy_index] - neighbor;
                    if (
                        point_in_field(neighbor) &&
                        len_point(delta) <= ENEMY_RADIUS
                    ) {
                        obstacles[enemy_index].push_back(neighbor);
                    }
                }
            }
        }
    }
}

void Monitor::generatePath() {
    auto transform_point = [&](QPointF point) {
        return Vec(
            point.x() - PADDING,
            HEIGHT + PADDING - point.y()
        );
    };
    generator->robot = transform_point(robot);
    generator->ball = transform_point(ball);
    generator->enemies.clear();
    for (auto &enemy : enemies) {
        generator->enemies.push_back(transform_point(enemy));
    }
    generator->obstacles.clear();
    for (auto &group_obstacles : obstacles){
        vector<Vec> new_obstacles;
        for (auto &obstacle : group_obstacles) {
            new_obstacles.push_back(transform_point(obstacle));
        }
        generator->obstacles.push_back(new_obstacles);
    }

    auto transform_points = [&](vector<Vec> points) {
        QVector<QPointF> result;
        for (auto &point : points) {
            result.push_back(transformPoint(point));
        }
        return result;
    };

    generator->generatePath();
    generator->generateSmoothPath(100);
    astar_path = transform_points(generator->astar_path);
    bezier_path = transform_points(generator->bezier_path);
}

void Monitor::clearPath() {
    astar_path.clear();
    bezier_path.clear();
}

void Monitor::updateWorldFile() {
    fstream file("webots_ws/worlds/soccer.wbt");
}