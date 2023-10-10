#include "monitor.hpp"

Monitor::Monitor(){
    setFixedSize(1170, 640);
    setWindowTitle("Robot State");

    generator = new PathGenerator();
    global = GlobalData::getInstance();
    
    updateData();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Monitor::updateDisplay);
    timer->start(10);

    pushButton[0] = new QPushButton("generate", this);
    pushButton[0]->setGeometry(950, 10, 100, 30);
    connect(pushButton[0], &QPushButton::clicked, this, &Monitor::generatePath);

    pushButton[1] = new QPushButton("clear", this);
    pushButton[1]->setGeometry(1060, 10, 100, 30);
    connect(pushButton[1], &QPushButton::clicked, this, &Monitor::clearPath);

    pushButton[2] = new QPushButton("save", this);
    pushButton[2]->setGeometry(950, 50, 100, 30);
    connect(pushButton[2], &QPushButton::clicked, [&]() {
        try {
            saveData();
            QMessageBox::information(this, "Success", "Success save data");
        } catch(...) {
            QMessageBox::information(this, "Error", "Failed save data");
        }
    });

    vBoxLayout[0] = new QVBoxLayout();
    vBoxLayout[0]->setContentsMargins(950, 90, 0, 0);
    vBoxLayout[0]->setAlignment(Qt::AlignTop);

    label[0] = new QLabel("Path Scenario: ", this);
    label[0]->setFont(QFont("Arial", 9));
    vBoxLayout[0]->addWidget(label[0]);

    buttonGroup[0] = new QButtonGroup(this);
    hBoxLayout = new QHBoxLayout();
    for (int i = 0; i < 5; i++) {
      radioButton[i] = new QRadioButton(QString::number(i), this);
      if (i == global->path_number) radioButton[i]->setChecked(true);

      buttonGroup[0]->addButton(radioButton[i]);
      buttonGroup[0]->setId(radioButton[i], i);
      hBoxLayout->addWidget(radioButton[i]);
    }
    connect(buttonGroup[0], &QButtonGroup::idClicked, this, [&](int id) {
      global->path_number = id;
      global->updateValue();
      updateData();
      if (isPathGenerated) generatePath();
    });
    hBoxLayout->setAlignment(Qt::AlignLeft);
    vBoxLayout[0]->addLayout(hBoxLayout);

    label[1] = new QLabel("Node Distance: ", this);
    label[1]->setFont(QFont("Arial", 9));
    vBoxLayout[0]->addWidget(label[1]);

    spinBox[0] = new QSpinBox(this);
    spinBox[0]->setMinimum(10);
    spinBox[0]->setMaximum(100);
    spinBox[0]->setValue(global->node_distance);
    spinBox[0]->setMaximumSize(QSize(100, 25));
    connect(spinBox[0], &QSpinBox::valueChanged, this, [&](int value) {
      global->node_distance = value;
      global->updateValue();
      updateData();
      if (isPathGenerated) generatePath();
    });
    vBoxLayout[0]->addWidget(spinBox[0]);

    label[2] = new QLabel("Robot Radius: ", this);
    label[2]->setFont(QFont("Arial", 9));
    vBoxLayout[0]->addWidget(label[2]);

    spinBox[1] = new QSpinBox(this);
    spinBox[1]->setMinimum(10);
    spinBox[1]->setMaximum(100);
    spinBox[1]->setValue(global->robot_radius/2);
    spinBox[1]->setMaximumSize(QSize(100, 25));
    connect(spinBox[1], &QSpinBox::valueChanged, this, [&](int value) {
      global->robot_radius = 2*value;
      global->updateValue();
      updateData();
      if (isPathGenerated) generatePath();
    });
    vBoxLayout[0]->addWidget(spinBox[1]);

    label[3] = new QLabel("Bezier Curvature: ", this);
    label[3]->setFont(QFont("Arial", 9));
    vBoxLayout[0]->addWidget(label[3]);

    spinBox[2] = new QSpinBox(this);
    spinBox[2]->setMinimum(1);
    spinBox[2]->setMaximum(10);
    spinBox[2]->setValue(global->bezier_curvature);
    spinBox[2]->setMaximumSize(QSize(100, 25));
    connect(spinBox[2], &QSpinBox::valueChanged, this, [&](int value) {
      global->bezier_curvature = value;
      global->updateValue();
      updateData();
      if (isPathGenerated) generatePath();
    });
    vBoxLayout[0]->addWidget(spinBox[2]);

    label[4] = new QLabel("Heuristic Function: ", this);
    label[4]->setFont(QFont("Arial", 9));
    vBoxLayout[0]->addWidget(label[4]);

    buttonGroup[1] = new QButtonGroup(this);
    vBoxLayout[1] = new QVBoxLayout();
    QString heuristic_name[] = {"Manhattan", "Diagonal", "Octile", "Euclidean"};
    for (int i = 5; i < 9; i++) {
      radioButton[i] = new QRadioButton(heuristic_name[i-5], this);
      if (i-4 == global->heuristic_type) radioButton[i]->setChecked(true);

      buttonGroup[1]->addButton(radioButton[i]);
      buttonGroup[1]->setId(radioButton[i], i-4);
      vBoxLayout[1]->addWidget(radioButton[i]);
    }
    connect(buttonGroup[1], &QButtonGroup::idClicked, this, [&](int id) {
      global->heuristic_type = id;
      global->updateValue();
      updateData();
      if (isPathGenerated) generatePath();
    });
    vBoxLayout[1]->setAlignment(Qt::AlignTop);
    vBoxLayout[0]->addLayout(vBoxLayout[1]);

    label[5] = new QLabel("Visited Node: 0", this);
    vBoxLayout[0]->addWidget(label[5]);

    label[6] = new QLabel("A* Length: 0", this);
    vBoxLayout[0]->addWidget(label[6]);

    label[7] = new QLabel("Bezier Length: 0", this);
    vBoxLayout[0]->addWidget(label[7]);

    label[8] = new QLabel("Time: 0", this);
    vBoxLayout[0]->addWidget(label[8]);

    label[9] = new QLabel("Error: 0", this);
    vBoxLayout[0]->addWidget(label[9]);    

    setLayout(vBoxLayout[0]);
}

Monitor::~Monitor() {
    delete generator;

    delete timer;
    for (int i = 0; i < 3; i++)
      delete pushButton[i];
    for (int i = 0; i < 9; i++)
      delete label[i];
    for (int i = 0; i < 2; i++)
      delete buttonGroup[i];
    delete hBoxLayout;
    for (int i = 0; i < 2; i++)
      delete vBoxLayout[i];
    for (int i = 0; i < 9; i++)
      delete radioButton[i];
    for (int i = 0; i < 2; i++)
      delete spinBox[i];
}

void Monitor::updateData() {
  robot = target = transformPoint(global->robot);
  ball = transformPoint(global->ball);
  enemies.clear();
  obstacles.clear();
  obstacles_visible.clear();

  for (auto &enemy : global->enemies) {
      enemies.push_back(transformPoint(enemy));
  }
  for (auto &group_obstacles : global->obstacles){
      QVector<QPointF> new_obstacles;
      for (auto &obstacle : group_obstacles) {
          new_obstacles.push_back(transformPoint(obstacle));
      }
      obstacles.push_back(new_obstacles);
  }
  for (auto &group_obstacles : global->obstacles_visible){
      QVector<QPointF> new_obstacles;
      for (auto &obstacle : group_obstacles) {
          new_obstacles.push_back(transformPoint(obstacle));
      }
      obstacles_visible.push_back(new_obstacles);
  }
}

QPointF Monitor::transformPoint(Vec point) {
    return QPointF{point.x + global->screen_padding, global->screen_height + global->screen_padding - point.y};
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

void Monitor::addFollowingPath(Vec point) {
  following_path.push_back(transformPoint(point));
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

void Monitor::setNodeVisited(int number) {
    label[5]->setText("Visited Node: " + QString::number(number));
}

void Monitor::setAStarLength(double length) {
    label[6]->setText("A* Length: " + QString::number(length));
}

void Monitor::setBezierLength(double length) {
    label[7]->setText("Bezier Length: " + QString::number(length));
}

void Monitor::setTimeFollow(double time) {
    label[8]->setText("Time: " + QString::number(time));
}

void Monitor::setFollowingError(double error) {
    label[9]->setText("Error: " + QString::number(error));
}

void Monitor::updateDisplay() {
    update();
}

void Monitor::paintEvent(QPaintEvent*) {
    QPainter painter(this);

    // draw field
    painter.fillRect(rect(), Qt::green);
    painter.fillRect(940, 0, 230, 640, Qt::white);
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
    painter.setBrush(Qt::yellow);
    for (int x = global->node_distance; x < global->screen_width; x += global->node_distance) {
      for (int y = global->node_distance; y < global->screen_height; y += global->node_distance) {
        painter.drawEllipse(transformPoint(Vec(x, y)), 2, 2);
      }
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawEllipse(robot, 10, 10);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(robot, global->robot_radius/2, global->robot_radius/2);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::cyan);
    painter.drawEllipse(ball, 10, 10);

    painter.setPen(Qt::black);
    painter.drawLine(robot.x(), robot.y(),
    robot.x() + 40 * cos(direction),
    robot.y() + 40 * sin(direction));

    for (int i = 0; i < enemies.size(); i++) {
        painter.setPen(Qt::red);
        painter.setBrush(Qt::red);
        painter.drawEllipse(enemies[i], 10, 10);
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(enemies[i], global->robot_radius/2, global->robot_radius/2);
        painter.setPen(QPen(Qt::darkBlue, 2, Qt::DashLine));
        painter.drawEllipse(enemies[i], global->robot_radius, global->robot_radius);
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::darkBlue);
    for (auto &group_obstacles : obstacles){
        for (auto &obstacle : group_obstacles) {
            painter.drawEllipse(obstacle, 2, 2);
        }
    }
    painter.setBrush(Qt::red);
    for (auto &group_obstacles : obstacles_visible){
        for (auto &obstacle : group_obstacles) {
            painter.drawEllipse(obstacle, 2, 2);
        }
    }
    painter.setBrush(Qt::darkRed);
    painter.setPen(Qt::darkRed);
    for (int i = 0; i < visited_node.size(); i++) {
        painter.drawEllipse(visited_node[i], 2, 2);
    }
    painter.setBrush(Qt::blue);
    painter.setPen(Qt::blue);
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
    painter.setBrush(Qt::darkGray);
    painter.setPen(Qt::darkGray);
    for (int i = 1; i < following_path.size(); i++) {
        painter.drawPoint(following_path[i]);
    }

    painter.setPen(Qt::cyan);
    painter.drawLine(robot, target);
}

void Monitor::mousePressEvent(QMouseEvent* event) {
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
            if (event->pos().x() >= enemies[i].x() - global->robot_radius &&
                event->pos().x() <= enemies[i].x() + global->robot_radius &&
                event->pos().y() >= enemies[i].y() - global->robot_radius &&
                event->pos().y() <= enemies[i].y() + global->robot_radius) {
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

void Monitor::mouseMoveEvent(QMouseEvent* event) {
    if (clicked) {
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
    if (clicked) {
        clicked = false;
        if (enemy_index < enemies.size()) {
            obstacles[enemy_index].clear();
            obstacles_visible[enemy_index].clear();
            QPointF temp_point{
                static_cast<int>((enemies[enemy_index].x() - global->screen_padding) / global->node_distance) * global->node_distance + global->screen_padding,
                static_cast<int>((enemies[enemy_index].y() - global->screen_padding) / global->node_distance) * global->node_distance + global->screen_padding
            };
            int offset = static_cast<int>(global->robot_radius / global->node_distance + 1);

            auto point_in_field = [&](QPointF point) {
                if (
                    point.x() >= global->screen_padding && point.x() <= global->screen_width &&
                    point.y() >= global->screen_padding && point.y() <= global->screen_height
                ) return true;
                return false;
            };

            auto len_point = [](QPointF point) {
                return sqrt(point.x()*point.x() + point.y()*point.y());
            };

            for (int i = -offset; i <= offset; i++) {
                for (int j = -offset; j <= offset; j++) {
                    QPointF neighbor = temp_point + QPointF{i * global->node_distance, j * global->node_distance};
                    QPointF delta = enemies[enemy_index] - neighbor;
                    if (point_in_field(neighbor)) {
                      if (len_point(delta) <= global->robot_radius)
                        obstacles[enemy_index].push_back(neighbor);
                      if (len_point(delta) <= global->robot_radius/2)
                        obstacles_visible[enemy_index].push_back(neighbor);
                    }
                }
            }
        }
    }
}

void Monitor::generatePath() {
    isPathGenerated = true;
    auto transform_point = [&](QPointF point) {
        return Vec(
            point.x() - global->screen_padding,
            global->screen_height + global->screen_padding - point.y()
        );
    };
    global->robot = transform_point(robot);
    global->ball = transform_point(ball);
    global->enemies.clear();
    for (auto &enemy : enemies) {
        global->enemies.push_back(transform_point(enemy));
    }
    global->obstacles.clear();
    for (auto &group_obstacles : obstacles){
        vector<Vec> new_obstacles;
        for (auto &obstacle : group_obstacles) {
            new_obstacles.push_back(transform_point(obstacle));
        }
        global->obstacles.push_back(new_obstacles);
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
    astar_path = transform_points(global->astar_path);
    bezier_path = transform_points(global->bezier_path);
    visited_node = transform_points(global->visited_node);

    setNodeVisited(global->visited_node.size());
    setAStarLength(generator->getAstarLength());
    setBezierLength(generator->getBezierLength());
}

void Monitor::clearPath() {
    isPathGenerated = false;
    astar_path.clear();
    bezier_path.clear();
    visited_node.clear();
}

void Monitor::saveData() {
    ifstream world_file_input(global->worlds_filename);
    vector<string> lines;
    string line;
    while (getline(world_file_input, line)) {
        lines.push_back(line);
    }
    world_file_input.close();
    vector<int> update_line{31, 35, 51, 60, 69, 78, 87};
    auto update_string_position = [&](string pos_str, QPointF pos) {
        stringstream ss(pos_str);
        vector<string> nums;
        string num;

        while (ss >> num) {
            nums.push_back(num);
        }

        double x_ = (pos.x() - global->screen_padding - 450) / 100;
        double y_ = (global->screen_height + global->screen_padding - pos.y() - 300) / 100;
        
        stringstream result;
        result << "  " << nums[0] << " " << x_ << " " << y_ << " " << nums[3];
        return result.str();
    };

    lines[update_line[0]] = update_string_position(lines[update_line[0]], ball);
    lines[update_line[1]] = update_string_position(lines[update_line[1]], robot);
    for (size_t i = 2; i < update_line.size(); i++) {
        lines[update_line[i]] = update_string_position(lines[update_line[i]], enemies[i-2]);
    }

    ofstream world_file_output(global->worlds_filename);
    for (auto line : lines) {
        world_file_output << line << endl;
    }
    world_file_output.close();

    ofstream global_file(global->global_filename);
    ofstream position_file(global->position_filename);
    
    global->global["path_number"] = global->path_number;
    global->global["heuristic_type"] = global->heuristic_type;
    global->global["robot_radius"] = global->robot_radius;
    global->global["node_distance"] = global->node_distance;
    global->global["bezier_curvature"] = global->bezier_curvature;
    
    auto convert_point = [&](QPointF point) {
      Vec point_ = Vec(
          point.x() - global->screen_padding,
          global->screen_height + global->screen_padding - point.y()
      );
      json data;
      data["x"] = point_.x / 100 - 4.5;
      data["y"] = point_.y / 100 - 3;
      return data;
    };

    global->position[global->path_number]["robot"] = convert_point(robot);
    global->position[global->path_number]["ball"] = convert_point(ball);
    json enemies_data;
    for (auto enemy : enemies) {
      enemies_data.push_back(convert_point(enemy));
    }
    global->position[global->path_number]["enemies"] = enemies_data;

    global_file << global->global;
    position_file << global->position;
    global_file.close();
    position_file.close();
}