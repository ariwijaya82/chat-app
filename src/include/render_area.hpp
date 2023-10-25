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

    void setMode();
    void setAnimate(bool);
    void setGeneratePath(bool);
    bool setPathNext(bool);
    void setModifiedPath(bool);
    void handlePanelChange(string widget, int value=0);

  protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

  private:
    QTimer *timer;
    GlobalData *global;
    PathGenerator *generator;

    QPoint mouseStart;
    QPoint mouseCurr;
    QPoint mouseRelease;
    QPoint mouseOffset;

    bool isMouseClicked = false;
    bool isMouseInStart = false;
    bool isMouseInTarget = false;
    bool isMouseInNode = false;
    bool isMouseInTarPos = false;
    bool isMouseInEnemy = false;
    bool isSelectEnemy = false;
    
    size_t index_enemy;
    size_t index_enemy_select;
    size_t index_i;
    size_t index_j;

    QPoint transformPoint(Vec);
    Vec transformPoint(QPoint);
};

#endif