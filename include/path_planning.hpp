#ifndef __PATH_PLANNING_HPP__
#define __PATH_PLANNING_HPP__

#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace std;
using nlohmann::json;

struct Coordinate {
    double x, y;

    bool operator==(Coordinate);
    Coordinate operator+(Coordinate);
    Coordinate operator-(Coordinate);
    Coordinate operator*(double);
    double dot(Coordinate);
    double len();
};
ostream& operator<<(ostream &os, Coordinate coord);

struct Node {
    double G, H;
    Coordinate coordinate;
    Node *parent;

    Node(Coordinate coord_, Node* parent_=nullptr);
    double getScore();
};


class PathPlanning {
    public:
        PathPlanning();
        pair<double, double> getStart();
        pair<double, double> getGoal();
        vector<pair<double, double>> getEnemy();
        vector<pair<double, double>> getCollision();
        vector<pair<double, double>> getPath();
        vector<pair<double, double>> getBezierPath();

    private:
        Coordinate start, goal;
        vector<Coordinate> enemy, walls, path, bezier_path;

        double HEIGHT, WIDTH, NODE_DISTANCE, ENEMY_RADIUS;
     
        Coordinate transformPoint(Coordinate);
        double heuristic(Coordinate, Coordinate, int);
        bool detectCollision(Coordinate);
        vector<Coordinate> getNeighbors(Coordinate);
        Node* findNodeOnList(vector<Node*>&, Coordinate);
        void releaseNodes(vector<Node*>&);
        
        vector<Coordinate> findPath();
        vector<Coordinate> generateBezier(int);

        Coordinate calculateBezierPoint(double);
};

#endif
