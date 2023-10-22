#ifndef __RENDER_AREA_HPP__
#define __RENDER_AREA_HPP__

#include <QtWidgets/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtCore/QTimer>

#include "utils.hpp"
#include "path_generator.hpp"

class RenderArea : public QWidget {
  public:
    RenderArea(GlobalData* global, QWidget* parent = nullptr);
    
    void render();

    void setMode(int);
    void setAnimate(bool);
    void setGeneratePath(bool);

  protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

  private:
    QTimer *timer;
    GlobalData *global;
    PathGenerator *generator;

    int mode = 0;

    QPoint transformPoint(Vec);
};

#endif