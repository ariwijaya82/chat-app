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
        PathGenerator(GlobalData* global_): global(global_) {}
        ~PathGenerator(){}

        void generatePath();
        void generateSmoothPath(int);

        double getAstarLength();
        double getBezierLength();
        int getTotalVisitedNode();

        bool astar_find_next_node();
        void astar_find_neighbors(bool ignore_head=false);
        void process_path();
        void modified_path(bool ignore_head=false);
        void getBezierPoints(int, int);

        vector<Node*> openList, closeList;
        Node* current;

    private:
        GlobalData* global;

        double heuristic(Vec, Vec, int);
        bool detectCollision(Vec);
        vector<Vec> getNeighbors(Vec, bool ignore_head=false);
        Node* findNodeOnList(vector<Node*>&, Vec);
        void releaseNodes(vector<Node*>&);
};

#endif
