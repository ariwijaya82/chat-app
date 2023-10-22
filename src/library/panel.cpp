#include "panel.hpp"

Panel::Panel(GlobalData* global) : global(global) {
  setFixedSize(1240, 640);

  render_area = new RenderArea(global, this);
  leftButton = new QPushButton("Generate", this);
  middleButton = new QPushButton("Start", this);
  rightButton = new QPushButton("Save", this);
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
  mainLayout->addWidget(render_area);

  QVBoxLayout *panelLayout = new QVBoxLayout();
  panelLayout->setContentsMargins(10, 10, 10, 10);
  panelLayout->setAlignment(Qt::AlignTop);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->addWidget(leftButton);
  buttonLayout->addWidget(middleButton);
  buttonLayout->addWidget(rightButton);
  panelLayout->addLayout(buttonLayout);

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
  connect(middleButton, &QPushButton::clicked, this, &Panel::handleMiddleButton);
  connect(rightButton, &QPushButton::clicked, this, &Panel::handleRightButton);

  connect(staticCheck, &QCheckBox::stateChanged, this, [&](int state) {
    // todo: handle this
  });
  connect(bezierSlider, &QSlider::sliderMoved, this, [&](int value) {
    // todo: handle this
  });

  pathCombo->addItems(QStringList{"1", "2", "3", "4", "5"});
  pathCombo->setCurrentIndex(global->path_number);
  connect(pathCombo, &QComboBox::currentIndexChanged, this, &Panel::handlePathCombo);

  heuristicCombo->addItems(QStringList{"Manhattan", "Diagonal", "Octile", "Euclidean"});
  heuristicCombo->setCurrentIndex(global->heuristic_type-1);
  connect(heuristicCombo, &QComboBox::currentIndexChanged, this, [&](int index) {
    global->heuristic_type = index + 1;
  });

  nodeSpin->setMinimum(10);
  nodeSpin->setMaximum(100);
  nodeSpin->setValue(global->node_distance);
  connect(nodeSpin, &QSpinBox::valueChanged, this, [&](int value) {
    global->node_distance = value;
    global->updateObstacles();

    render_area->render();

  });

  radiusSpin->setMinimum(10);
  radiusSpin->setMaximum(100);
  radiusSpin->setValue(global->robot_radius/2);
  connect(radiusSpin, &QSpinBox::valueChanged, this, [&](int value) {
    global->robot_radius = value * 2;
    global->updateObstacles();

    render_area->render();

  });

  bezierSpin->setMinimum(1);
  bezierSpin->setMaximum(10);
  bezierSpin->setValue(global->bezier_curvature);
  connect(bezierSpin, &QSpinBox::valueChanged, this, [&](int value) {
    global->bezier_curvature = value;
  });
}

void Panel::setMode(int mode_) {
  mode = mode_;
  render_area->setMode(mode);
  switch (mode)
  {
  case 0:
    leftButton->setVisible(true);
    middleButton->setVisible(true);
    rightButton->setVisible(true);
    staticCheck->setVisible(true);

    leftButton->setText("Generate");
    middleButton->setText("Start");
    rightButton->setText("Save");

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
    break;

  case 1:
    leftButton->setVisible(true);
    middleButton->setVisible(true);
    rightButton->setVisible(true);
    staticCheck->setVisible(true);

    leftButton->setText("Connect");
    middleButton->setText("Start");
    rightButton->setText("Save");

    bezierSlider->setVisible(false);
    pathCombo->setVisible(false);
    heuristicCombo->setVisible(false);
    nodeSpin->setVisible(false);
    radiusSpin->setVisible(false);
    bezierSpin->setVisible(false);
    pathLabel->setVisible(false);
    heuristicLabel->setVisible(false);
    nodeLabel->setVisible(false);
    radiusLabel->setVisible(false);
    bezierSpinLabel->setVisible(false);
    bezierSliderLabel->setVisible(false);
    break;

  case 2:
    leftButton->setVisible(true);
    middleButton->setVisible(true);
    rightButton->setVisible(false);
    staticCheck->setVisible(false);

    leftButton->setText("Next");
    middleButton->setText("Reset");

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
    break;

  case 3:
    leftButton->setVisible(true);
    middleButton->setVisible(false);
    rightButton->setVisible(false);
    staticCheck->setVisible(false);

    leftButton->setText("Reset");

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
    break;
  
  default:
    break;
  }
}

void Panel::handleLeftButton() {
  global->path_number = 4;
  global->updatePosition();
  global->updateObstacles();

  render_area->render();
}

void Panel::handleMiddleButton() {
  
}

void Panel::handleRightButton() {
  
}

void Panel::handlePathCombo(int value) {
  global->path_number = value;
  global->updatePosition();
  global->updateObstacles();

  render_area->render();
}

void Panel::handleHeuristicCombo(int value) {
  
}