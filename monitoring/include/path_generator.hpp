#ifndef __PATH_GENERATOR_HPP__
#define __PATH_GENERATOR_HPP__

#include <vector>
#include <algorithm>
#include <cmath>
#include <nlohmann/json.hpp>

#include "utils.hpp"
#include "point.hpp"

using namespace std;
using nlohmann::json;

typedef struct Node {
    double G;
    double H;
    Point pos;
    Node* parent;
} node;

class PathGenerator {
    public:
    PathGenerator();
    // set value
    void setRobotPos();
    void setBallPos();
    void setEnemyPos(int);
    // process
    void generatePath();
    // get value
    
    private:
    int node_distance;
    int heuristic_type;
    int bezier_curvature;
    int robot_radius;
    int screen_width;
    int screen_height;

    Point ball;
    Point robot;
    vector<Point> enemies;
    vector<vector<Point>> obstacles;

    // helper function
    void updateObstacles(int);
};

#endif