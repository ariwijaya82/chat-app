#include "main_window.hpp"

MainWindow::MainWindow(GlobalData* global) {
  setWindowTitle("Robot Monitor");
  setFixedSize(1240, 660);

  createActions();
  createMenus();
  connectActions();

  panel = new Panel(global);
  setCentralWidget(panel);
}

void MainWindow::createActions() {
  simulateAct = new QAction("Simulate", this);
  simulateAct->setCheckable(true);
  simulateAct->setChecked(true);
  astarAct = new QAction("AStar", this);
  astarAct->setCheckable(true);
  bezierAct = new QAction("Bezier", this);
  bezierAct->setCheckable(true);

  fieldAct = new QAction("Field", this);
  fieldAct->setCheckable(true);
  fieldAct->setChecked(true);
  nodeAct = new QAction("Node", this);
  nodeAct->setCheckable(true);
  nodeAct->setChecked(true);
  visNodeAct = new QAction("Visitied Node", this);
  visNodeAct->setCheckable(true);
  visNodeAct->setChecked(true);
  robotAreaAct = new QAction("Robot Area", this);
  robotAreaAct->setCheckable(true);
  robotAreaAct->setChecked(true);
  astarPathAct = new QAction("AStar Path", this);
  astarPathAct->setCheckable(true);
  astarPathAct->setChecked(true);
  bezierPathAct = new QAction("Bezier Path", this);
  bezierPathAct->setCheckable(true);
  bezierPathAct->setChecked(true);
}

void MainWindow::createMenus() {
  modeMenu = menuBar()->addMenu("Mode");
  modeMenu->addAction(simulateAct);
  modeMenu->addAction(astarAct);
  modeMenu->addAction(bezierAct);

  viewMenu = menuBar()->addMenu("View");
  viewMenu->addAction(fieldAct);
  viewMenu->addAction(nodeAct);
  viewMenu->addAction(visNodeAct);
  viewMenu->addAction(robotAreaAct);
  viewMenu->addAction(astarPathAct);
  viewMenu->addAction(bezierPathAct);
}

void MainWindow::connectActions() {
  connect(simulateAct, &QAction::triggered, this, [&]() {
    handleModeChanged(0);
  });
  connect(astarAct, &QAction::triggered, this, [&]() {
    handleModeChanged(1);
  });
  connect(bezierAct, &QAction::triggered, this, [&]() {
    handleModeChanged(2);
  });

  connect(fieldAct, &QAction::triggered, this, [&](bool checked) {
    fieldAct->setChecked(checked);
    handleViewChanged(0, checked);
  });
  connect(nodeAct, &QAction::triggered, this, [&](bool checked) {
    nodeAct->setChecked(checked);
    handleViewChanged(1, checked);
  });
  connect(visNodeAct, &QAction::triggered, this, [&](bool checked) {
    visNodeAct->setChecked(checked);
    handleViewChanged(2, checked);
  });
  connect(robotAreaAct, &QAction::triggered, this, [&](bool checked) {
    robotAreaAct->setChecked(checked);
    handleViewChanged(3, checked);
  });
  connect(astarPathAct, &QAction::triggered, this, [&](bool checked) {
    astarPathAct->setChecked(checked);
    handleViewChanged(4, checked);
  });
  connect(bezierPathAct, &QAction::triggered, this, [&](bool checked) {
    bezierPathAct->setChecked(checked);
    handleViewChanged(5, checked);
  });
}

void MainWindow::handleModeChanged(int index) {
  simulateAct->setChecked(false);
  astarAct->setChecked(false);
  bezierAct->setChecked(false);

  panel->setMode(index);

  switch (index) {
    case 0: {
      simulateAct->setChecked(true);
      viewMenu->setEnabled(true);
      break;
    }

    case 1: {
      astarAct->setChecked(true);
      viewMenu->setEnabled(false);
      break;
    }

    case 2: {
      bezierAct->setChecked(true);
      viewMenu->setEnabled(false);
      break;
    }
  }
}

void MainWindow::handleViewChanged(int index, bool check) {
  // todo: implement view change
}