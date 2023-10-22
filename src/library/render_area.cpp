#include "render_area.hpp"

#include <iostream>
using namespace std;

RenderArea::RenderArea(GlobalData* global, QWidget* parent) : QWidget(parent), global(global) {
  setFixedSize(940 ,640);

  timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &RenderArea::render);
  
  generator = new PathGenerator(global);
}

void RenderArea::render() {
  update();
}

void RenderArea::setMode(int value) {
  mode = value;
  update();
}

void RenderArea::setGeneratePath(bool value) {
  if (value) {
    generator->generatePath();
    generator->generateSmoothPath(100);
    update();
  } else {
    global->astar_path.clear();
    global->bezier_path.clear();
  }
}

void RenderArea::setAnimate(bool value) {
  if (value) timer->start(50);
  else timer->stop();
}

void RenderArea::paintEvent(QPaintEvent* paintEvent) {
  QPainter painter(this);
  switch (mode)
  {
  case 0:
  case 1:
    painter.fillRect(rect(), Qt::green);
    painter.setPen(Qt::white);
    painter.drawRect(20, 20, 900, 600);
    painter.drawRect(20, 70, 200, 500);
    painter.drawRect(20, 170, 100, 300);
    painter.drawRect(720, 70, 200, 500);
    painter.drawRect(820, 170, 100, 300);
    painter.drawEllipse(QPoint(470, 320), 100, 100);
    painter.drawLine(470, 20, 470, 620);

    painter.setPen(Qt::yellow);
    painter.setBrush(Qt::yellow);
    for (int x = global->node_distance; x < global->screen_width; x += global->node_distance) {
      for (int y = global->node_distance; y < global->screen_height; y += global->node_distance) {
        painter.drawEllipse(transformPoint(Vec(x, y)), 2, 2);
      }
    }

    painter.setPen(Qt::red);
    painter.setBrush(Qt::red);
    for (auto& data : global->obstacles) {
      for (auto& point : data) {
        painter.drawEllipse(transformPoint(point), 2, 2);
      }
    }

    painter.setPen(Qt::black);
    painter.setBrush(Qt::black);
    painter.drawEllipse(transformPoint(global->robot), 5, 5);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(transformPoint(global->robot), (int)global->robot_radius/2, (int)global->robot_radius/2);
    painter.setPen(Qt::white);
    painter.setBrush(Qt::white);
    painter.drawEllipse(transformPoint(global->ball), 5, 5);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(transformPoint(global->ball), (int)global->robot_radius/2, (int)global->robot_radius/2);
    for (auto& enemy : global->enemies) {
      painter.setPen(Qt::red);
      painter.setBrush(Qt::red);
      painter.drawEllipse(transformPoint(enemy), 5, 5);
      painter.setBrush(Qt::NoBrush);
      painter.drawEllipse(transformPoint(enemy), (int)global->robot_radius/2, (int)global->robot_radius/2);
      painter.setPen(QPen(Qt::red, 1, Qt::DashLine));
      painter.drawEllipse(transformPoint(enemy), (int)global->robot_radius, (int)global->robot_radius);
    }
    break;

  case 2:
    painter.fillRect(rect(), Qt::green);

    painter.setPen(Qt::yellow);
    painter.setBrush(Qt::yellow);
    for (int x = global->node_distance; x < global->screen_width; x += global->node_distance) {
      for (int y = global->node_distance; y < global->screen_height; y += global->node_distance) {
        painter.drawEllipse(transformPoint(Vec(x, y)), 2, 2);
      }
    }

    painter.setPen(Qt::red);
    painter.setBrush(Qt::red);
    for (auto& data : global->obstacles) {
      for (auto& point : data) {
        painter.drawEllipse(transformPoint(point), 2, 2);
      }
    }
    break;
  
  default:
    painter.fillRect(rect(), Qt::green);

    painter.setPen(Qt::yellow);
    painter.setBrush(Qt::yellow);
    for (int x = global->node_distance; x < global->screen_width; x += global->node_distance) {
      for (int y = global->node_distance; y < global->screen_height; y += global->node_distance) {
        painter.drawEllipse(transformPoint(Vec(x, y)), 2, 2);
      }
    }

    break;
  }
}

QPoint RenderArea::transformPoint(Vec point) {
  return QPoint(point.x + 20, 620 - point.y);
}

void RenderArea::mousePressEvent(QMouseEvent*) {
  
}

void RenderArea::mouseMoveEvent(QMouseEvent*) {
  
}

void RenderArea::mouseReleaseEvent(QMouseEvent*) {
  
}
