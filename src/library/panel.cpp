#include "panel.hpp"

Panel::Panel(GlobalData* global) : global(global) {
  setFixedSize(1240, 640);

  renderArea = new RenderArea(global, this);
  for (int i = 0; i < 6; i++) {
    robotSocket[i] = new QWebSocket();
  }

  timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &Panel::handleTimer);

  leftButton = new QPushButton(this);
  rightButton = new QPushButton(this);
  connectButton = new QPushButton(this);
  startButton = new QPushButton(this);

  staticCheck = new QCheckBox("Static robot", this);
  bezierSlider = new QSlider(Qt::Horizontal, this);
  pathCombo = new QComboBox(this);
  heuristicCombo = new QComboBox(this);
  nodeSpin = new QSpinBox(this);
  radiusSpin = new QSpinBox(this);
  bezierSpin = new QSpinBox(this);
  pathLabel = new QLabel("Path:", this);
  heuristicLabel = new QLabel("Cost function:", this);
  nodeLabel = new QLabel("Node distance:", this);
  radiusLabel = new QLabel("Robot radius:", this);
  bezierSpinLabel = new QLabel("Bezier curvature:", this);
  bezierSliderLabel = new QLabel("Bezier (t):", this);

  QHBoxLayout *mainLayout = new QHBoxLayout();
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->addWidget(renderArea);

  QVBoxLayout *panelLayout = new QVBoxLayout();
  panelLayout->setContentsMargins(10, 10, 10, 10);
  panelLayout->setAlignment(Qt::AlignTop);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->addWidget(leftButton);
  buttonLayout->addWidget(rightButton);
  panelLayout->addLayout(buttonLayout);

  QHBoxLayout *buttonLayoutSecond = new QHBoxLayout();
  buttonLayoutSecond->addWidget(connectButton);
  buttonLayoutSecond->addWidget(startButton);
  panelLayout->addLayout(buttonLayoutSecond);

  panelLayout->addWidget(staticCheck);
  panelLayout->addWidget(bezierSliderLabel);
  panelLayout->addWidget(bezierSlider);

  QFormLayout *formLayout = new QFormLayout();
  formLayout->addRow(pathLabel, pathCombo);
  formLayout->addRow(heuristicLabel, heuristicCombo);
  formLayout->addRow(nodeLabel, nodeSpin);
  formLayout->addRow(radiusLabel, radiusSpin);
  formLayout->addRow(bezierSpinLabel, bezierSpin);
  panelLayout->addLayout(formLayout);

  mainLayout->addLayout(panelLayout);
  setLayout(mainLayout);

  global->isStatic = true;
  setMode(0);

  connect(leftButton, &QPushButton::clicked, this, &Panel::handleLeftButton);
  connect(rightButton, &QPushButton::clicked, this, &Panel::handleRightButton);
  connect(connectButton, &QPushButton::clicked, this, &Panel::handleConnectButton);
  connect(startButton, &QPushButton::clicked, this, &Panel::handleStartButton);

  connect(staticCheck, &QCheckBox::stateChanged, this, [&](int value) { renderArea->handlePanelChange("staticCheck", value); });

  bezierSlider->setMinimum(0);
  bezierSlider->setMaximum(100);
  connect(bezierSlider, &QSlider::sliderMoved, this, [&](int value) { renderArea->handlePanelChange("bezierSlider", value); });

  pathCombo->addItems(QStringList{"1", "2", "3", "4", "5"});
  pathCombo->setCurrentIndex(global->path_number);
  connect(pathCombo, &QComboBox::currentIndexChanged, this, [&](int value) { renderArea->handlePanelChange("pathCombo", value); });

  heuristicCombo->addItems(QStringList{"Manhattan", "Chebyshev", "Octile", "Euclidean"});
  heuristicCombo->setCurrentIndex(global->heuristic_type-1);
  connect(heuristicCombo, &QComboBox::currentIndexChanged, this, [&](int value) { renderArea->handlePanelChange("heuristicCombo", value); });

  nodeSpin->setMinimum(10);
  nodeSpin->setMaximum(200);
  nodeSpin->setValue(global->node_distance);
  connect(nodeSpin, &QSpinBox::valueChanged, this, [&](int value) { renderArea->handlePanelChange("nodeSpin", value); });

  radiusSpin->setMinimum(10);
  radiusSpin->setMaximum(100);
  radiusSpin->setValue(global->robot_radius/2);
  connect(radiusSpin, &QSpinBox::valueChanged, this, [&](int value) { renderArea->handlePanelChange("radiusSpin", value); });

  bezierSpin->setMinimum(1);
  bezierSpin->setMaximum(10);
  bezierSpin->setValue(global->bezier_curvature);
  connect(bezierSpin, &QSpinBox::valueChanged, this, [&](int value) { renderArea->handlePanelChange("bezierSpin", value); });
}

void Panel::setMode(int mode) {
  global->mode = mode;
  renderArea->setMode();
  switch (global->mode) {
    case 0: {
      leftButton->setVisible(true);
      rightButton->setVisible(true);
      connectButton->setVisible(true);
      startButton->setVisible(true);

      staticCheck->setVisible(true);
      bezierSlider->setVisible(false);
      pathCombo->setVisible(true);
      heuristicCombo->setVisible(true);
      nodeSpin->setVisible(true);
      radiusSpin->setVisible(true);
      bezierSpin->setVisible(true);

      pathLabel->setVisible(true);
      heuristicLabel->setVisible(true);
      nodeLabel->setVisible(true);
      radiusLabel->setVisible(true);
      bezierSpinLabel->setVisible(true);
      bezierSliderLabel->setVisible(false);
      
      leftButton->setText("Generate");
      rightButton->setText("Save");
      connectButton->setText("Connect");
      startButton->setText("Start");
      
      staticCheck->setCheckState(global->isStatic ? Qt::Checked : Qt::Unchecked);
      pathCombo->setCurrentIndex(global->path_number);
      heuristicCombo->setCurrentIndex(global->heuristic_type-1);
      nodeSpin->setValue(global->node_distance);
      radiusSpin->setValue(global->robot_radius/2);
      bezierSpin->setValue(global->bezier_curvature);
      
      leftButton->setEnabled(true);
      startButton->setEnabled(false);
      connectButton->setEnabled(true);
      heuristicCombo->setEnabled(true);
      nodeSpin->setEnabled(true);
      try {
        for (int i = 0; i < 6; i++) {
          robotSocket[i]->close();
        }
      } catch (...) {
        cout << "failed to disconnect socket" << endl;
      }
      break;
    }

    case 1: {
      leftButton->setVisible(true);
      rightButton->setVisible(true);
      connectButton->setVisible(false);
      startButton->setVisible(false);

      staticCheck->setVisible(false);
      bezierSlider->setVisible(false);
      pathCombo->setVisible(false);
      heuristicCombo->setVisible(true);
      nodeSpin->setVisible(true);
      radiusSpin->setVisible(false);
      bezierSpin->setVisible(false);
      
      pathLabel->setVisible(false);
      heuristicLabel->setVisible(true);
      nodeLabel->setVisible(true);
      radiusLabel->setVisible(false);
      bezierSpinLabel->setVisible(false);
      bezierSliderLabel->setVisible(false);

      leftButton->setText("Next");
      rightButton->setText("Reset");

      heuristicCombo->setCurrentIndex(global->heuristic_type-1);
      nodeSpin->setValue(global->node_distance);
      
      leftButton->setEnabled(true);
      heuristicCombo->setEnabled(true);
      nodeSpin->setEnabled(true);
      break;
    }

    case 2: {
      leftButton->setVisible(true);
      rightButton->setVisible(false);
      connectButton->setVisible(false);
      startButton->setVisible(false);

      staticCheck->setVisible(false);
      bezierSlider->setVisible(true);
      pathCombo->setVisible(false);
      heuristicCombo->setVisible(false);
      nodeSpin->setVisible(true);
      radiusSpin->setVisible(false);
      bezierSpin->setVisible(true);
      
      pathLabel->setVisible(false);
      heuristicLabel->setVisible(false);
      nodeLabel->setVisible(true);
      radiusLabel->setVisible(false);
      bezierSpinLabel->setVisible(true);
      bezierSliderLabel->setVisible(true);
      
      leftButton->setText("Generate");
      
      nodeSpin->setValue(global->node_distance);
      bezierSlider->setValue(0);
      
      leftButton->setEnabled(true);
      bezierSlider->setEnabled(false);
      nodeSpin->setEnabled(true);
      break;
    }
  }

}

void Panel::setView(int index, bool check) {
  switch (index) {
    case 0:
      global->showNode = check;
      break;
    
    case 1:
      global->showVisNode = check;
      break;
    
    case 2:
      global->showRobotArea = check;
      break;
    
    case 3:
      global->showAstarPath = check;
      break;
    
    case 4:
      global->showBezierPath = check;
      break;
    case 5:
      global->showFollowingPath = check;
      break;
  }
  renderArea->render();
}

void Panel::handleLeftButton() {
  switch (global->mode) {
    case 0: {
      if (global->isGenerate) {
        global->isGenerate = false;
        leftButton->setText("Generate");
        renderArea->setGeneratePath(false);
      } else {
        global->isGenerate = true;
        leftButton->setText("Reset");
        renderArea->setGeneratePath(true);
      }
      break;
    }
    
    case 1: {
      if (global->ball == Vec(-50, -50)) {
        QMessageBox::information(this, "Warning", "Please select start and target points");
        break;
      }

      if (renderArea->setPathNext(true)) {
        leftButton->setEnabled(false);
      }
      heuristicCombo->setEnabled(false);
      nodeSpin->setEnabled(false);
      break;
    }

    case 2: {
      if (global->isGenerate) {
        global->isGenerate = false;
        leftButton->setText("Generate");
        bezierSlider->setEnabled(false);
        bezierSlider->setValue(0);
        renderArea->setModifiedPath(false);
        nodeSpin->setEnabled(true);
      } else {
        if (global->astar_path.size() < 3) {
          QMessageBox::information(this, "Warning", "Please select at least 3 points");
          break;
        }
        global->isGenerate = true;
        leftButton->setText("Reset");
        bezierSlider->setEnabled(true);
        renderArea->setModifiedPath(true);
        nodeSpin->setEnabled(false);
      }
      break;
    }
  }
  renderArea->render();
}

void Panel::handleRightButton() {
  switch (global->mode) {
    case 0: {
      try {
        if (global->isConnected) {
          global->saveTargetPosition();
        } else {
          global->saveValue();
        }
        QMessageBox::information(this, "Success", "Success save data");
      } catch(...) {
        QMessageBox::information(this, "Error", "Failed save data");
      }
      break;
    }

    case 1: {
      renderArea->setPathNext(false);
      leftButton->setEnabled(true);
      heuristicCombo->setEnabled(true);
      nodeSpin->setEnabled(true);
      break;
    }
  }
  renderArea->render();
}

void Panel::handleConnectButton() {
  if (global->isConnected) {
    try {
      for (int i = 0; i < 6; i++) {
        robotSocket[i]->close();
      }
    } catch (...) {
      cout << "failed to disconnect socket" << endl;
    }
  } else {
    try {
      connect(robotSocket[0], &QWebSocket::connected, this, [&](){
        renderArea->setMode();

        pathCombo->setCurrentIndex(global->path_number);
        heuristicCombo->setCurrentIndex(global->heuristic_type-1);
        nodeSpin->setValue(global->node_distance);
        radiusSpin->setValue(global->robot_radius/2);
        bezierSpin->setValue(global->bezier_curvature);

        leftButton->setText("Generate");
        connectButton->setText("Disconnect");
        leftButton->setEnabled(false);
        startButton->setEnabled(true);
        staticCheck->setEnabled(false);
        pathCombo->setEnabled(false);
        heuristicCombo->setEnabled(false);
        nodeSpin->setEnabled(false);
        radiusSpin->setEnabled(false);
        bezierSpin->setEnabled(false);

        global->isConnected = true;
        global->connected[0] = true;
        renderArea->render();
      });
      connect(robotSocket[1], &QWebSocket::connected, this, [&](){ global->connected[1] = true; renderArea->render(); });
      connect(robotSocket[2], &QWebSocket::connected, this, [&](){ global->connected[2] = true; renderArea->render(); });
      connect(robotSocket[3], &QWebSocket::connected, this, [&](){ global->connected[3] = true; renderArea->render(); });
      connect(robotSocket[4], &QWebSocket::connected, this, [&](){ global->connected[4] = true; renderArea->render(); });
      connect(robotSocket[5], &QWebSocket::connected, this, [&](){ global->connected[5] = true; renderArea->render(); });

      connect(robotSocket[0], &QWebSocket::disconnected, this, [&](){
        if (global->isConnected) {
          renderArea->setMode();
          
          connectButton->setText("Connect");
          startButton->setText("Start");
          startButton->setEnabled(false);
          connectButton->setEnabled(true);
          staticCheck->setEnabled(true);
          pathCombo->setEnabled(true);
          heuristicCombo->setEnabled(true);
          nodeSpin->setEnabled(true);
          radiusSpin->setEnabled(true);
          bezierSpin->setEnabled(true);
          leftButton->setEnabled(true);
          
          global->isConnected = false;
          global->isStart = false;
          global->connected[0] = false;
          renderArea->render();
        }
      });
      connect(robotSocket[1], &QWebSocket::disconnected, this, [&](){ global->connected[1] = false; renderArea->render(); });
      connect(robotSocket[2], &QWebSocket::disconnected, this, [&](){ global->connected[2] = false; renderArea->render(); });
      connect(robotSocket[3], &QWebSocket::disconnected, this, [&](){ global->connected[3] = false; renderArea->render(); });
      connect(robotSocket[4], &QWebSocket::disconnected, this, [&](){ global->connected[4] = false; renderArea->render(); });
      connect(robotSocket[5], &QWebSocket::disconnected, this, [&](){ global->connected[5] = false; renderArea->render(); });
      
      connect(robotSocket[0], &QWebSocket::textMessageReceived, this, [&](const QString& message) { handleSocketMessage(0, message.toStdString()); }); 
      connect(robotSocket[1], &QWebSocket::textMessageReceived, this, [&](const QString& message) { handleSocketMessage(1, message.toStdString()); }); 
      connect(robotSocket[2], &QWebSocket::textMessageReceived, this, [&](const QString& message) { handleSocketMessage(2, message.toStdString()); }); 
      connect(robotSocket[3], &QWebSocket::textMessageReceived, this, [&](const QString& message) { handleSocketMessage(3, message.toStdString()); }); 
      connect(robotSocket[4], &QWebSocket::textMessageReceived, this, [&](const QString& message) { handleSocketMessage(4, message.toStdString()); }); 
      connect(robotSocket[5], &QWebSocket::textMessageReceived, this, [&](const QString& message) { handleSocketMessage(5, message.toStdString()); }); 
    
      for (int i = 0; i < 6; i++) {
        QString url = "ws://127.0.0.1:" + QString::number(9000+i);
        robotSocket[i]->open(QUrl(url));
      }
    } catch (...) {
      cout << "failed to connect socket" << endl;
    }
  }
}

void Panel::handleStartButton() {
  if (global->isStart) {
    global->isStart = false;
    startButton->setText("Start");
    connectButton->setEnabled(true);
    timer->stop();

    json data;
    data["type"] = "run";
    data["value"] = "stop";
    for (int i = 0; i < 6; i++) {
      robotSocket[i]->sendTextMessage(QString(to_string(data).c_str()));
    }
  } else {
    global->isStart = true;
    startButton->setText("Stop");
    connectButton->setEnabled(false);
    timer->start(50);
    
    json data;
    data["type"] = "run";
    data["value"] = "start";

    robotSocket[0]->sendTextMessage(QString(to_string(data).c_str()));
    for (int i = 1; i < 6; i++) {
      if (!global->isStatic && global->target_index[i-1] < global->target_position[i-1].size()) {
        Vec point = global->target_position[i-1][global->target_index[i-1]];
        data["target"]["x"] = point.x;
        data["target"]["y"] = point.y;
        data["value"] = "start";
      } else data["value"] = "stop";
      robotSocket[i]->sendTextMessage(QString(to_string(data).c_str()));
    }
  }
}

void Panel::handleTimer() {
  renderArea->render();
  if (global->isStart) {
    global->timer += timer->interval();
    global->interval += timer->interval();
    if (!global->isStatic && global->interval >= 3000) {
      global->interval = 0;
      global->updateObstacles();

      json data;
      data["type"] = "update";
      data["value"] = json::array();
      for (auto &obstacles : global->obstacles) {
        json arr = json::array();
        for (auto &obstacle : obstacles) {
          json temp;
          temp["x"] = obstacle.x;
          temp["y"] = obstacle.y;
          arr.push_back(temp);
        }
        data["value"].push_back(arr);
      }
      robotSocket[0]->sendTextMessage(QString(to_string(data).c_str()));
    }
  }
}

void Panel::handleSocketMessage(int i, string message) {
  json recv_data = json::parse(message);
  string type = recv_data["type"].template get<string>();
  if (i == 0) { // robot
    if (type == "position") {
      global->robot = Vec(
        recv_data["value"]["x"].template get<double>(),
        recv_data["value"]["y"].template get<double>()
      );
      global->following_path.push_back(global->robot);
      global->target = Vec(
        recv_data["value"]["target"]["x"].template get<double>(),
        recv_data["value"]["target"]["y"].template get<double>()
      );
      global->direction[0] = recv_data["value"]["dir"].template get<double>();
    } else if (type == "astar_path") {
      global->astar_path.clear();
      for (auto &item : recv_data["value"]) {
        global->astar_path.push_back(Vec(
          item["x"].template get<double>(),
          item["y"].template get<double>()
        ));
      }
    } else if (type == "bezier_path") {
      global->bezier_path.clear();
      global->normal_bezier_path.clear();
      for (auto &item : recv_data["value"]) {
        global->bezier_path.push_back(Vec(
          item["x"].template get<double>(),
          item["y"].template get<double>()
        ));
      }
    } else if (type == "finished") {
      global->isStart = false;
      startButton->setEnabled(false);
      connectButton->setEnabled(true);
      // global->actual_path.push_back(global->robot);
    }
  } else { // enemy
    if (type == "position") {
      global->enemies[i-1] = Vec(
        recv_data["value"]["x"].template get<double>(),
        recv_data["value"]["y"].template get<double>()
      );
      global->direction[i] = recv_data["value"]["dir"].template get<double>();
    } else if (type == "finished") {
      if (global->target_index[i-1] < global->target_position[i-1].size()) {
        Vec point = global->target_position[i-1][global->target_index[i-1]];
        int target_x = recv_data["target"]["x"].template get<int>();
        int target_y = recv_data["target"]["y"].template get<int>();
        if (abs(target_x-point.x) < global->robot_radius/2 && abs(target_y-point.y) < global->robot_radius/2)
          global->target_index[i-1]++;
        json data;
        data["type"] = "target";
        data["value"]["x"] = point.x;
        data["value"]["y"] = point.y;
        robotSocket[i]->sendTextMessage(QString(to_string(data).c_str()));
      } else {
        json data;
        data["type"] = "run";
        data["value"] = "stop";
        robotSocket[i]->sendTextMessage(QString(to_string(data).c_str()));
      }
    }
  }
}