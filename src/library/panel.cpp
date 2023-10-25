#include "panel.hpp"

Panel::Panel(GlobalData* global) : global(global) {
  setFixedSize(1240, 640);

  renderArea = new RenderArea(global, this);
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

  heuristicCombo->addItems(QStringList{"Manhattan", "Diagonal", "Octile", "Euclidean"});
  heuristicCombo->setCurrentIndex(global->heuristic_type-1);
  connect(heuristicCombo, &QComboBox::currentIndexChanged, this, [&](int value) { renderArea->handlePanelChange("heuristicCombo", value); });

  nodeSpin->setMinimum(10);
  nodeSpin->setMaximum(100);
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
      bezierSlider->setEnabled(false);
      leftButton->setEnabled(true);
      break;
    }
  }

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
      if (!global->isGenerate) leftButton->setEnabled(true);
      if (renderArea->setPathNext(true)) {
        leftButton->setEnabled(false);
      }
      break;
    }

    case 2: {
      if (global->isGenerate) {
        global->isGenerate = false;
        leftButton->setText("Generate");
        bezierSlider->setEnabled(false);
        bezierSlider->setValue(0);
        renderArea->setModifiedPath(false);
      } else {
        if (global->astar_path.size() < 3) {
          QMessageBox::information(this, "Warning", "Please select at least 3 points");
          break;
        }
        global->isGenerate = true;
        leftButton->setText("Reset");
        bezierSlider->setEnabled(true);
        renderArea->setModifiedPath(true);
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
        global->saveValue();
        QMessageBox::information(this, "Success", "Success save data");
      } catch(...) {
        QMessageBox::information(this, "Error", "Failed save data");
      }
      break;
    }

    case 1: {
      renderArea->setPathNext(false);
      leftButton->setEnabled(true);
      break;
    }
  }
  renderArea->render();
}

void Panel::handleConnectButton() {
  if (global->isConnected) {
    global->isConnected = false;
    connectButton->setText("Connect");
  } else {
    global->isConnected = true;
    connectButton->setText("Disconnect");
  }
}

void Panel::handleStartButton() {
  if (global->isStart) {
    global->isStart = false;
    startButton->setText("Start");
  } else {
    global->isStart = true;
    startButton->setText("Stop");
  }
}