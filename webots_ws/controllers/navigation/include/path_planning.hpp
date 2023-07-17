#ifndef __PATH_PLANNING_HPP__
#define __PATH_PLANNING_HPP__

#include <vector>

using namespace std;

struct Coordinate {
    int x, y;
    bool operator==(const Coordinate&);
};

struct Node {
    int G, H;
    Coordinate coordinate;
    Node *parent;

    Node(Coordinate coord_, Node* parent_=nullptr);
    int getScore();
};


class PathPlanning {
    public:
        PathPlanning();
        pair<int, int> getStart();
        pair<int, int> getGoal();
        vector<pair<int, int>> getEnemy();
        vector<pair<int, int>> getCollision();
        vector<pair<int, int>> getPath();
        vector<pair<int, int>> getBezierPath();

    private:
        Coordinate start, goal;
        vector<Coordinate> enemy, walls, path, bezier_path;

        int heuristic(Coordinate, Coordinate);
        bool detectCollision(Coordinate);
        Node* findNodeOnList(vector<Node*>&, Coordinate);
        void releaseNodes(vector<Node*>&);

        Coordinate transformPoint(vector<double>&);
        Coordinate calculateBezierPoint(double);
        vector<Coordinate> findPath();
        vector<Coordinate> generateBezier(int);
};

#endif
