#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

#include <nlohmann/json.hpp>

using namespace std;
using nlohmann::json;

class Vec {
  public:
    Vec(double x=0, double y=0);

    double x = 0, y = 0;
    double len();

    Vec operator+(Vec);
    Vec operator-(Vec);
    Vec operator*(double);
    Vec operator/(double);
    bool operator==(Vec);
};

ostream& operator<<(ostream &os, Vec vec);

class GlobalData {
  public:

    GlobalData(string);
    void loadFile();
    void updateGlobal();
    void updatePosition();
    void updateObstacles();
    void updateTargetPosition();
    void saveValue();

    string global_filename;
    string position_filename;
    string worlds_filename;

    double screen_height,
           screen_width,
           screen_padding,
           robot_radius,
           node_distance;
    int heuristic_type, path_number, bezier_curvature;
    
    Vec robot, ball;
    vector<Vec> enemies;
    
    vector<Vec> visited_node, astar_path, bezier_path;
    vector<vector<Vec>> obstacles, obstacles_visible, target_position;

  private:
    string dir;
    json global, position;
};

#endif