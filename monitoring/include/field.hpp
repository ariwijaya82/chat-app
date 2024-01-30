#ifndef __FIELD_HPP__
#define __FIELD_HPP__

#include <QtWidgets/QWidget>
#include <QtGui/QPainter>

#include <vector>
#include <map>

#include "utils.hpp"

using namespace std;

class Field : public QWidget {
    public:
    Field(QWidget* parent=nullptr);

    protected:
    void paintEvent(QPaintEvent*) override;

    private:
    int mode;

    int node_distance;
    int robot_radius;
    int screen_width;
    int screen_height;
    
    QPointF ball;
    QPointF robot;
    vector<QPointF> enemies;
    vector<vector<QPointF>> obstacles;

    vector<double> direction;
    map<string, bool> show;
};

#endif