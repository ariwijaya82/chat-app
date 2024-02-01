#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

#include <nlohmann/json.hpp>

using namespace std;
using nlohmann::json;
// Vec class
class Vec {
  public:
    double x = 0, y = 0;
    Vec(double x=0, double y=0);  
    double len();
    // Vec class operator
    Vec operator+(Vec);
    Vec operator-(Vec);
    Vec operator*(double);
    Vec operator/(double);
    bool operator==(Vec);
};
// Vec cout overload function
ostream& operator<<(ostream &os, Vec vec);
// GlobalData class
class GlobalData {
  public:
    GlobalData(string);
    // public function
    void loadFile();
    void updateGlobal();
    void updatePosition();
    void updateObstacles();
    void updateTargetPosition();
    void saveValue();
    void saveTargetPosition();
    // public data
    string global_filename;
    string position_filename;
    string worlds_filename;
    double screen_height;
    double screen_width;
    double screen_padding;
    double robot_radius;
    double node_distance;
    int path_number;
    int heuristic_type;
    int bezier_curvature;
    // robot data
    Vec robot;
    Vec ball;
    Vec target;
    vector<Vec> enemies;
    vector<vector<Vec>> obstacles;
    vector<vector<Vec>> obstacles_visible;
    vector<vector<Vec>> target_position;
    vector<vector<Vec>> control_points;
    vector<Vec> visited_node;
    vector<Vec> astar_path;
    vector<Vec> normal_astar_path;
    vector<Vec> modified_astar_path;
    vector<Vec> bezier_path;
    vector<Vec> normal_bezier_path;
    vector<Vec> following_path;
    vector<Vec> actual_path;
    // flag data
    int mode;
    bool isGenerate;
    bool isStart;
    bool isStatic;
    bool isConnected;
    bool connected[6];
    double direction[6];
    size_t target_index[5];
    bool showNode = true;
    bool showVisNode = true;
    bool showRobotArea = true;
    bool showAstarPath = true;
    bool showBezierPath = true;
    bool showFollowingPath = true;
    bool showRobot = true;
    // timer data
    int timer = 0;
    int interval;

  private:
    string dir;
    json global;
    json position;
};

#endif