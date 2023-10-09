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
    string global_filename = "../../config/global.json";
    string position_filename = "../../config/position.json";
    string worlds_filename = "../../webots_ws/worlds/soccer.wbt";

    ~GlobalData() { delete m_UniqueInstance; }
    static GlobalData* getInstance() { return m_UniqueInstance; }
    void updateValue();

    json global, position;

    double screen_height,
           screen_width,
           screen_padding,
           robot_radius,
           node_distance;
    int heuristic_type, path_number, bezier_curvature;
    
    Vec robot, ball;
    vector<Vec> enemies, visited_node, astar_path, bezier_path;
    vector<vector<Vec>> obstacles, obstacles_visible;

  private:
    static GlobalData* m_UniqueInstance;

    GlobalData();
    GlobalData(const GlobalData&) = delete;
    GlobalData& operator=(const GlobalData&) = delete;
};

// Constant getConstant();
double smoothValue(double value, double min_input, double max_input, double min_output, double max_output);

#endif