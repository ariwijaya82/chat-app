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
    bool operator==(Vec);
};
ostream& operator<<(ostream &os, Vec vec);

class Constant {
    public:
    Constant();
    
    double height, width, padding, radius, distance;
    Vec robot, ball;
    vector<Vec> enemies;
    vector<vector<Vec>> obstacles;
};

#endif