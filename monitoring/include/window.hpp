#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMenu>
#include <QtWidgets/QHBoxLayout>

#include <QtGui/QAction>
#include <QtGui/QPainter>
#include <vector>

#include "field.hpp"
#include "panel.hpp"
#include "utils.hpp"

using namespace std;

class Window : public QMainWindow {
    public:
    Window();

    private:
    QMenu* mode;
    QMenu* view;
    vector<QAction*> modeAction;
    vector<QAction*> viewAction;
    
    QWidget* mainWidget;
    Field* field;
    Panel* panel;

    QHBoxLayout* mainLayout;
};

#endif