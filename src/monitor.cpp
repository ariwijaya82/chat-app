#include "monitor.hpp"

Monitor::Monitor(GlobalData* global_) {
    setFixedSize(1250, 640);
    setWindowTitle("Robot Monitoring");

    generator = new PathGenerator(global_);
    global = global_;

    updateData();
    displayPanel();

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Monitor::updateDisplay);
    timer->start(10);

    for (int i = 0; i < 6; i++) {
      if (i == 0) direction[i] = 0;
      else direction[i] = M_PI;
      connected[i] = false;
    }
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


void Monitor::updateDisplay() {
    update();
}

void Monitor::displayPanel() {
  QVBoxLayout *mainLayout = new QVBoxLayout();
  mainLayout->setAlignment(Qt::AlignTop);
  mainLayout->setContentsMargins(950, 10, 10, 10);

  QHBoxLayout *modeLayout = new QHBoxLayout();
  QLabel *modeLabel = new QLabel("Mode: ", this);
  QComboBox *modeList = new QComboBox(this);
  modeList->addItems(QStringList{"Simulation", "Remote", "A-star", "Bezier Curve"});
  modeLayout->setAlignment(Qt::AlignLeft);
  modeLayout->addWidget(modeLabel);
  modeLayout->addWidget(modeList);
  mainLayout->addLayout(modeLayout);
  connect(modeList, &QComboBox::currentIndexChanged, this, Monitor::handleModeChange);

  QHBoxLayout *pathLayout = new QHBoxLayout();
  pathLabel = new QLabel("Path Scenario: ", this);
  pathList = new QComboBox(this);
  pathList->addItems(QStringList{"1", "2", "3", "4", "5"});
  pathList->setCurrentIndex(global->path_number);
  pathLayout->setAlignment(Qt::AlignLeft);
  pathLayout->addWidget(pathLabel);
  pathLayout->addWidget(pathList);
  mainLayout->addLayout(pathLayout);
  connect(pathList, &QComboBox::currentIndexChanged, this, [&](int index) {
    global->path_number = index;
    global->updatePosition();
    global->updateObstacles();
    updateData();
    if (isGenerate) generatePath();
  });

  QHBoxLayout *heuristicLayout = new QHBoxLayout();
  heuristicLabel = new QLabel("Cost Function: ", this);
  heuristicList = new QComboBox(this);
  heuristicList->addItems(QStringList{"Manhattan", "Diagonal", "Octile", "Euclidean"});
  heuristicList->setCurrentIndex(global->heuristic_type-1);
  heuristicLayout->setAlignment(Qt::AlignLeft);
  heuristicLayout->addWidget(heuristicLabel);
  heuristicLayout->addWidget(heuristicList);
  mainLayout->addLayout(heuristicLayout);
  connect(heuristicList, &QComboBox::currentIndexChanged, this, [&](int index) {
    global->heuristic_type = index;
    if (isGenerate) generatePath();
  });

  QHBoxLayout *nodeLayout = new QHBoxLayout();
  nodeLabel = new QLabel("Node Distance: ", this);
  nodeNumber = new QSpinBox(this);
  nodeNumber->setMinimum(10);
  nodeNumber->setMaximum(100);
  nodeNumber->setValue(global->node_distance);
  nodeLayout->setAlignment(Qt::AlignLeft);
  nodeLayout->addWidget(nodeLabel);
  nodeLayout->addWidget(nodeNumber);
  mainLayout->addLayout(nodeLayout);
  connect(nodeNumber, &QSpinBox::valueChanged, this, [&](int value) {
    global->node_distance = value;
    global->updateObstacles();
    updateData();
    if (isGenerate) generatePath();
  });

  QHBoxLayout *radiusLayout = new QHBoxLayout();
  radiusLabel = new QLabel("Robot Radius: ", this);
  radiusNumber = new QSpinBox(this);
  radiusNumber->setMinimum(10);
  radiusNumber->setMaximum(100);
  radiusNumber->setValue(global->robot_radius);
  radiusLayout->setAlignment(Qt::AlignLeft);
  radiusLayout->addWidget(radiusLabel);
  radiusLayout->addWidget(radiusNumber);
  mainLayout->addLayout(radiusLayout);
  connect(radiusNumber, &QSpinBox::valueChanged, this, [&](int value) {
    global->robot_radius = value;
    global->updateObstacles();
    updateData();
    if (isGenerate) generatePath();
  });

  QHBoxLayout *curvatureLayout = new QHBoxLayout();
  curvatureLabel = new QLabel("Curvature: ", this);
  curvatureNumber = new QSpinBox(this);
  curvatureNumber->setMinimum(1);
  curvatureNumber->setMaximum(10);
  curvatureNumber->setValue(global->bezier_curvature);
  curvatureLayout->setAlignment(Qt::AlignLeft);
  curvatureLayout->addWidget(curvatureLabel);
  curvatureLayout->addWidget(curvatureNumber);
  mainLayout->addLayout(curvatureLayout);
  connect(curvatureNumber, &QSpinBox::valueChanged, this, [&](int value) {
    global->bezier_curvature = value;
    if (isGenerate) generatePath();
  });

  QHBoxLayout *bezierLayout = new QHBoxLayout();
  bezierLabel = new QLabel("Bezier: ", this);
  bezierSlider = new QSlider(this);
  bezierSlider->setTickInterval(100);
  bezierLayout->setAlignment(Qt::AlignLeft);
  bezierLayout->addWidget(bezierLabel);
  bezierLayout->addWidget(bezierSlider);
  mainLayout->addLayout(bezierLayout);
  connect(bezierSlider, &QSlider::valueChanged, this, [&](int value) {
    // todo: implement this
  });

  staticCheckbox = new QCheckBox("static", this);
  mainLayout->addWidget(staticCheckbox);
  connect(staticCheckbox, &QCheckBox::stateChanged, this, [&](int state) {
    // todo: implement this
  });

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  leftButton = new QPushButton("Generate", this);
  rightButton = new QPushButton("Start", this);
  buttonLayout->addWidget(leftButton);
  buttonLayout->addWidget(rightButton);
  mainLayout->addLayout(buttonLayout);
  connect(leftButton, &QPushButton::clicked, this, &Monitor::handleLeftButton);
  connect(rightButton, &QPushButton::clicked, &Monitor::handleRightButton);

  QHBoxLayout *buttonLayout2 = new QHBoxLayout();
  leftButton = new QPushButton("Save", this);
  buttonLayout2->addWidget(bottomButton);
  mainLayout->addLayout(buttonLayout2);
  connect(bottomButton, &QPushButton::clicked, this, &Monitor::handleBottomButton);

  setLayout(mainLayout);
}

void Monitor::handleModeChange(int index) {
  mode = index;
  
  switch (mode)
  {
  case 0: // simulation
    pathLabel->setVisible(true);
    pathList->setVisible(true);
    heuristicLabel->setVisible(true);
    heuristicList->setVisible(true);
    nodeLabel->setVisible(true);
    nodeNumber->setVisible(true);
    radiusLabel->setVisible(true);
    radiusNumber->setVisible(true);
    curvatureLabel->setVisible(true);
    curvatureNumber->setVisible(true);
    bezierLabel->setVisible(false);
    bezierSlider->setVisible(false);
    staticCheckbox->setVisible(true);
    leftButton->setVisible(true);
    rightButton->setVisible(true);
    bottomButton->setVisible(true);
    leftButton->setText("Generate");
    rightButton->setText("Start");
    bottomButton->setText("Save");
    break;

  case 1: // remote
    pathLabel->setVisible(false);
    pathList->setVisible(false);
    heuristicLabel->setVisible(false);
    heuristicList->setVisible(false);
    nodeLabel->setVisible(false);
    nodeNumber->setVisible(false);
    radiusLabel->setVisible(false);
    radiusNumber->setVisible(false);
    curvatureLabel->setVisible(false);
    curvatureNumber->setVisible(false);
    bezierLabel->setVisible(false);
    bezierSlider->setVisible(false);
    staticCheckbox->setVisible(false);
    leftButton->setVisible(false);
    rightButton->setVisible(false);
    bottomButton->setVisible(false);
    leftButton->setText("Connect");
    rightButton->setText("Start");
    bottomButton->setText("Save");
    break;
  
  case 2: // a star
    pathLabel->setVisible(false);
    pathList->setVisible(false);
    heuristicLabel->setVisible(true);
    heuristicList->setVisible(true);
    nodeLabel->setVisible(true);
    nodeNumber->setVisible(true);
    radiusLabel->setVisible(false);
    radiusNumber->setVisible(false);
    curvatureLabel->setVisible(false);
    curvatureNumber->setVisible(false);
    bezierLabel->setVisible(false);
    bezierSlider->setVisible(false);
    staticCheckbox->setVisible(false);
    leftButton->setVisible(true);
    rightButton->setVisible(true);
    bottomButton->setVisible(false);
    leftButton->setText("Next");
    rightButton->setText("Reset");

    isStart = false;
    break;

  case 3: // bezier
    pathLabel->setVisible(false);
    pathList->setVisible(false);
    heuristicLabel->setVisible(false);
    heuristicList->setVisible(false);
    nodeLabel->setVisible(false);
    nodeNumber->setVisible(false);
    radiusLabel->setVisible(false);
    radiusNumber->setVisible(false);
    curvatureLabel->setVisible(false);
    curvatureNumber->setVisible(false);
    bezierLabel->setVisible(true);
    bezierSlider->setVisible(true);
    staticCheckbox->setVisible(false);
    leftButton->setVisible(false);
    rightButton->setVisible(false);
    bottomButton->setVisible(true);
    bottomButton->setText("Reset");
    break;

  default:
    break;
  }
}

void Monitor::handleLeftButton() {
  switch (mode)
  {
  case 0: // simulation
    if (isGenerate) {
      isGenerate = false;
      leftButton->setText("Generate");

      astar_path.clear();
      bezier_path.clear();
      visited_node.clear();
      
      global->updatePosition(); 
      global->updateObstacles();
      for (int i = 0; i < 6; i++) {
        if (i == 0) direction[i] = 0;
        else direction[i] = M_PI;
        connected[i] = false;
      }
      updateData();
    } else {
      isGenerate = true;
      leftButton->setText("Reset");

      generatePath();
    }
    break;

  case 1: // remote
    if (connected[0]) {
      leftButton->setText("Connect");
      robotSocket[0]->close();
      robotSocket[1]->close();
      robotSocket[2]->close();
      robotSocket[3]->close();
      robotSocket[4]->close();
      robotSocket[5]->close();
    } else {
      if (robotSocket[0] == nullptr) {
        for (int i = 0; i < 6; i++) {
          robotSocket[i] = new QWebSocket();
          QString url = "ws://127.0.0.1:" + QString::number(9000+i);
          robotSocket[i]->open(QUrl(url));
        }
      }
      connect(robotSocket[0], &QWebSocket::connected, this, [&]() {
        connected[0] = true;
        leftButton->setText("Disconnect");
      });
      connect(robotSocket[1], &QWebSocket::connected, this, [&]() { connected[1] = true; });
      connect(robotSocket[2], &QWebSocket::connected, this, [&]() { connected[2] = true; });
      connect(robotSocket[3], &QWebSocket::connected, this, [&]() { connected[3] = true; });
      connect(robotSocket[4], &QWebSocket::connected, this, [&]() { connected[4] = true; });
      connect(robotSocket[5], &QWebSocket::connected, this, [&]() { connected[5] = true; });

      connect(robotSocket[0], &QWebSocket::disconnected, this, [&]() { connected[0] = false; });
      connect(robotSocket[1], &QWebSocket::disconnected, this, [&]() { connected[1] = false; });
      connect(robotSocket[2], &QWebSocket::disconnected, this, [&]() { connected[2] = false; });
      connect(robotSocket[3], &QWebSocket::disconnected, this, [&]() { connected[3] = false; });
      connect(robotSocket[4], &QWebSocket::disconnected, this, [&]() { connected[4] = false; });
      connect(robotSocket[5], &QWebSocket::disconnected, this, [&]() { connected[5] = false; });
      
      connect(robotSocket[0], &QWebSocket::textMessageReceived, this, &Monitor::handleWebsocketMessage);
      connect(robotSocket[1], &QWebSocket::textMessageReceived, this, &Monitor::handleWebsocketMessage);
      connect(robotSocket[2], &QWebSocket::textMessageReceived, this, &Monitor::handleWebsocketMessage);
      connect(robotSocket[3], &QWebSocket::textMessageReceived, this, &Monitor::handleWebsocketMessage);
      connect(robotSocket[4], &QWebSocket::textMessageReceived, this, &Monitor::handleWebsocketMessage);
      connect(robotSocket[5], &QWebSocket::textMessageReceived, this, &Monitor::handleWebsocketMessage);  
    }
    break;
  
  case 2: // a star
    if (!isStart) {
      isStart = true;
      generator->current = nullptr;
      global->visited_node.clear();
      
      generator->openList.clear();
      generator->closeList.clear();

      if ((global->robot - global->ball).len() < global->robot_radius) {
        global->astar_path = vector<Vec>{global->robot, global->ball};
        return;
      }

      generator->openList.push_back(new Node(global->robot));
    }

    generator->astar_find_next_node();
    generator->astar_find_neighbors();

    if (generator->openList.empty()) {
      global->visited_node.clear();
      global->astar_path = vector<Vec>{global->robot, global->ball};
      return;
    }

    generator->process_path();
    break;

  default:
    break;
  }
}

void Monitor::handleRightButton() {
  switch (mode)
  {
  case 0: // simulation
    // todo: implement this
  break;
  
  case 1: // remote
    // todo: implement this
  break;

  case 2: // a star
    // todo: implement this
  break;

  default:
    break;
  }
}

void Monitor::handleBottomButton() {
  if (mode == 0 || mode == 1) {

  } else if (mode == 2) {

  }
}

void Monitor::generatePath() {
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
}


void Monitor::handleWebsocketMessage(const QString& json_message) {
  json data = json::parse(json_message.toStdString());
  int id = data["id"].template get<int>();
  string type = data["type"].template get<string>();
  if (type == "position" && id == 0) {
    Vec pos{
      data["value"]["x"].template get<double>(),
      data["value"]["y"].template get<double>()
    };
    // setRobotPosition(pos);
  }
}



QPointF Monitor::transformPoint(Vec point) {
    return QPointF{point.x + global->screen_padding, global->screen_height + global->screen_padding - point.y};
};

void Monitor::paintEvent(QPaintEvent*) {
    QPainter painter(this);

    // draw field
    painter.fillRect(rect(), Qt::green);
    painter.fillRect(940, 0, 310, 640, Qt::white);
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

    painter.setPen(Qt::yellow);
    painter.setBrush(Qt::yellow);
    for (int x = global->node_distance; x < global->screen_width; x += global->node_distance) {
      for (int y = global->node_distance; y < global->screen_height; y += global->node_distance) {
        painter.drawEllipse(transformPoint(Vec(x, y)), 2, 2);
      }
    }

    painter.setPen(Qt::black);
    painter.setBrush(Qt::black);
    painter.drawEllipse(robot, 10, 10);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(robot, global->robot_radius/2, global->robot_radius/2);
    painter.setPen(Qt::white);
    painter.setBrush(Qt::white);
    painter.drawEllipse(ball, 10, 10);
    painter.setPen(Qt::black);
    painter.drawLine(robot.x(), robot.y(),
      robot.x() + global->robot_radius / 2 * cos(direction[0]),
      robot.y() + global->robot_radius / 2 * sin(direction[0]));
    if (connected[0]) painter.setPen(Qt::blue);
    else painter.setPen(Qt::black);
    painter.drawText(robot.x(), robot.y()-15, "robot test");

    for (int i = 0; i < enemies.size(); i++) {
        painter.setPen(Qt::red);
        painter.setBrush(Qt::red);
        painter.drawEllipse(enemies[i], 10, 10);
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(enemies[i], global->robot_radius/2, global->robot_radius/2);
        painter.setPen(QPen(Qt::red, 1, Qt::DashLine));
        painter.drawEllipse(enemies[i], global->robot_radius, global->robot_radius);
        painter.setPen(Qt::red);
        painter.drawLine(enemies[i].x(), enemies[i].y(),
          enemies[i].x() + global->robot_radius / 2 * cos(direction[i+1]),
          enemies[i].y() + global->robot_radius / 2 * sin(direction[i+1]));

        if (connected[i+1]) painter.setPen(Qt::blue);
        else painter.setPen(Qt::black);
        painter.drawText(enemies[i].x(), enemies[i].y()-15, "robot " + QString::number(i+1));
    }

    painter.setPen(Qt::darkRed);
    painter.setBrush(Qt::darkRed);
    for (auto &group_obstacles : obstacles){
        for (auto &obstacle : group_obstacles) {
            painter.drawEllipse(obstacle, 2, 2);
        }
    }
    painter.setPen(Qt::red);
    painter.setBrush(Qt::red);
    for (auto &group_obstacles : obstacles_visible){
        for (auto &obstacle : group_obstacles) {
            painter.drawEllipse(obstacle, 2, 2);
        }
    }
    painter.setPen(Qt::gray);
    painter.setBrush(Qt::gray);
    for (int i = 0; i < visited_node.size(); i++) {
        painter.drawEllipse(visited_node[i], 2, 2);
    }
    painter.setPen(Qt::blue);
    painter.setBrush(Qt::blue);
    for (int i = 0; i < astar_path.size(); i++) {
        painter.drawEllipse(astar_path[i], 2, 2);
        if (i != 0) {
            painter.drawLine(astar_path[i], astar_path[i-1]);
        }
    }
    painter.setPen(Qt::magenta);
    painter.setBrush(Qt::magenta);
    for (int i = 1; i < bezier_path.size(); i++) {
        painter.drawLine(bezier_path[i], bezier_path[i-1]);
    }
    painter.setPen(Qt::cyan);
    painter.setBrush(Qt::cyan);
    for (int i = 1; i < following_path.size(); i++) {
        painter.drawPoint(following_path[i]);
    }

    painter.setPen(Qt::darkGray);
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
