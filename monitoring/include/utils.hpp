#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <vector>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <QtCore/QPoint>

#include "point.hpp"

using namespace std;
using nlohmann::json;

typedef struct {
    int node_distance;
    int heuristic_type;
    int bezier_curvature;
    int robot_radius;
    int path_number;
    int screen_width;
    int screen_height;
    int screen_padding;
} param;

typedef struct {
    Point ball;
    Point robot;
    vector<Point> enemies;
    vector<vector<Point>> target;
} position;

param loadParameter();
position loadPosition(int);

Point convertJSONtoPoint(json);
vector<Point> getPointAround(Point, int, int);

QPointF convertCartesianToQt(Point);
Point convertQtToCartesian(QPointF);

#endif