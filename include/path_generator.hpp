#ifndef __PATH_GENERATOR_HPP__
#define __PATH_GENERATOR_HPP__

#include <vector>
#include <algorithm>
#include <cmath>
#include <nlohmann/json.hpp>

#include "utils.hpp"

using namespace std;
using nlohmann::json;

struct Node {
    double G, H;
    Vec coordinate;
    Node *parent;

    Node(Vec, Node* parent_=nullptr);
    double getScore();
};

class PathGenerator {
    public:
        PathGenerator();
        ~PathGenerator();

        Vec robot, ball;
        vector<Vec> enemies, astar_path, bezier_path;
        vector<vector<Vec>> obstacles;

        void generatePath();
        void generateSmoothPath(int);
    private:
        Constant* constant;
        double HEIGHT, WIDTH, NODE_DISTANCE;
     
        double heuristic(Vec, Vec, int);
        bool detectCollision(Vec);
        vector<Vec> getNeighbors(Vec);
        Node* findNodeOnList(vector<Node*>&, Vec);
        void releaseNodes(vector<Node*>&);
};

#endif
