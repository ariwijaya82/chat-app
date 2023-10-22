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

    double screen_height;
    double screen_width;
    double screen_padding;

    double robot_radius;
    double node_distance;
    int path_number;
    int heuristic_type;
    int bezier_curvature;
    
    Vec robot;
    Vec ball;
    vector<Vec> enemies;
    
    vector<vector<Vec>> obstacles;
    vector<vector<Vec>> obstacles_visible;
    vector<vector<Vec>> target_position;
    
    vector<Vec> visited_node;
    vector<Vec> astar_path;
    vector<Vec> bezier_path;
    vector<Vec> following_path;

    json global, position;
  private:
    string dir;
};

#endif