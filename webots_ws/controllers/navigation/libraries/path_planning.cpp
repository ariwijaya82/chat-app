#include "path_planning.hpp"
#include "constants.hpp"
#include "utils.hpp"
#include "bezier.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits.h>

bool Coordinate::operator==(const Coordinate& coord_) {
    return (x == coord_.x && y == coord_.y);
}

Coordinate operator+(const Coordinate& coord_1, const Coordinate& coord_2) {
    return {coord_1.x + coord_2.x, coord_1.y + coord_2.y};
}

Node::Node(Coordinate coordinate_, Node* parent_) {
    parent = parent_;
    coordinate = coordinate_;
    G = H = 0;
}

int Node::getScore(){
    return G + H; 
}

PathPlanning::PathPlanning() {
    vector<vector<double>> points = utils::read_position_csv();
    start = transformPoint(points[1]);
    goal = transformPoint(points[0]);
    for (int i = 2; i <= 6; i++) {
        enemy.push_back(transformPoint(points[i]));
    }

    for (int x = Constants::NODE_DISTANCE; x < Constants::WIDTH; x += Constants::NODE_DISTANCE) {
      for (int y = Constants::NODE_DISTANCE; y < Constants::HEIGHT; y += Constants::NODE_DISTANCE) {
        for (auto item : enemy) {
          double dis = std::sqrt(pow(x - item.x, 2) + pow(y - item.y, 2));
          if (dis < 50.0){
            walls.push_back(Coordinate{x, y});
          }
        }
      }
    }

    path = findPath();
    bezier_path = generateBezier();
}

pair<int, int> PathPlanning::getStart() {
    return pair<int, int>{start.x, start.y};
}

pair<int, int> PathPlanning::getGoal() {
    return pair<int, int>{goal.x, goal.y};
}

vector<pair<int, int>> PathPlanning::getEnemy() {
    vector<pair<int, int>> result;
    for (auto &item : enemy) {
        result.push_back(pair<int, int>{item.x, item.y});
    }
    return result;
}

vector<pair<int, int>> PathPlanning::getCollision() {
    vector<pair<int, int>> result;
    for (auto &item : walls) {
        result.push_back(pair<int, int>{item.x, item.y});
    }
    return result;
}

vector<pair<int, int>> PathPlanning::getPath() {
    vector<pair<int, int>> result;
    for (auto &item : path) {
        result.push_back(pair<int, int>{item.x, item.y});
    }
    return result;
}

vector<pair<int, int>> PathPlanning::getBezierPath() {
    vector<pair<int, int>> result;
    for (auto &item : bezier_path) {
        result.push_back(pair<int, int>{item.x, item.y});
    }
    return result;
}

int PathPlanning::heuristic(Coordinate source_, Coordinate target_) {
    Coordinate delta = {target_.x-source_.x, target_.y-source_.y};
    return static_cast<int>(sqrt(pow(delta.x, 2) + pow(delta.y, 2)));
}

bool PathPlanning::detectCollision(Coordinate coord_) {
    if (coord_.x < 0 || coord_.x >= Constants::WIDTH ||
        coord_.y < 0 || coord_.y >= Constants::HEIGHT ||
        std::find(walls.begin(), walls.end(), coord_) != walls.end()) {
        return true;
    }
    return false;
}

Node* PathPlanning::findNodeOnList(vector<Node*>& nodes_, Coordinate coordinate_)
{
    for (auto node : nodes_) {
        if (node->coordinate == coordinate_) {
            return node;
        }
    }
    return nullptr;
}

void PathPlanning::releaseNodes(vector<Node*>& nodes_) {
    for (int i = 0; i < nodes_.size(); i++)
        delete nodes_[i];
}

Coordinate PathPlanning::transformPoint(vector<double>& point) {
    int x = (point[0] + 4.5) * 100;
    int y = (point[1] + 3) * 100;
    return Coordinate{x, y};
}

vector<Coordinate> PathPlanning::findPath() {
    Node* current = nullptr;
    vector<Node*> openList, closeList;
    openList.reserve(100);
    closeList.reserve(100);

    int temp_x = (int)(start.x / Constants::NODE_DISTANCE) * Constants::NODE_DISTANCE;
    int temp_y = (int)(start.y / Constants::NODE_DISTANCE) * Constants::NODE_DISTANCE;
    vector<Coordinate> start_area {
        Coordinate{temp_x, temp_y},
        Coordinate{temp_x+Constants::NODE_DISTANCE, temp_y},
        Coordinate{temp_x, temp_y+Constants::NODE_DISTANCE},
        Coordinate{temp_x+Constants::NODE_DISTANCE, temp_y+Constants::NODE_DISTANCE},
    };
    temp_x = (int)(goal.x / Constants::NODE_DISTANCE) * Constants::NODE_DISTANCE;
    temp_y = (int)(goal.y / Constants::NODE_DISTANCE) * Constants::NODE_DISTANCE;
    vector<Coordinate> goal_area {
        Coordinate{temp_x, temp_y},
        Coordinate{temp_x+Constants::NODE_DISTANCE, temp_y},
        Coordinate{temp_x, temp_y+Constants::NODE_DISTANCE},
        Coordinate{temp_x+Constants::NODE_DISTANCE, temp_y+Constants::NODE_DISTANCE},
    };

    Coordinate start_point, goal_point;
    int min_value = INT_MAX;
    for (int i = 0; i < 4; i++) {
      int value = sqrt(pow(start_area[i].x - start.x, 2)+pow(start_area[i].y - start.y, 2))
        + sqrt(pow(start_area[i].x - goal.x, 2)+pow(start_area[i].y - goal.y, 2));
      if (min_value > value) {
        min_value = value;
        start_point = start_area[i];
      }
    }
    min_value = INT_MAX;
    for (int i = 0; i < 4; i++) {
      int value = sqrt(pow(goal_area[i].x - start.x, 2)+pow(goal_area[i].y - start.y, 2))
        + sqrt(pow(goal_area[i].x - goal.x, 2)+pow(goal_area[i].y - goal.y, 2));
      if (min_value > value) {
        min_value = value;
        goal_point = goal_area[i];
      }
    }

    vector<Coordinate> directions = {
        { 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 },
        { -1, -1 }, { 1, 1 }, { -1, 1 }, { 1, -1 }
    };
    for (auto &item : directions) {
        item.x *= Constants::NODE_DISTANCE;
        item.y *= Constants::NODE_DISTANCE;
    }
    openList.push_back(new Node(start_point));
    while (!openList.empty()) {
        auto current_it = openList.begin();
        current = *current_it;
        for (auto it = openList.begin(); it != openList.end(); it++) {
            auto node = *it;
            if (node->getScore() < current->getScore()) {
                current = node;
                current_it = it;
            }
        }

        if (current->coordinate == goal_point) break;

        closeList.push_back(current);
        openList.erase(current_it);

        for (int i = 0; i < directions.size(); i++) {
            Coordinate newCoord(current->coordinate + directions[i]);
            if (detectCollision(newCoord) || findNodeOnList(closeList, newCoord)) continue;
            int totalCost = current->G + ((i < 4) ? 1 : 1.4) * Constants::NODE_DISTANCE;

            Node* successor = findNodeOnList(openList, newCoord);
            if (successor == nullptr) {
                successor = new Node(newCoord, current);
                successor->G = totalCost;
                successor->H = heuristic(successor->coordinate, goal_point);
                openList.push_back(successor);
            } else if (totalCost < successor->G) {
                successor->parent = current;
                successor->G = totalCost;
            }
        }
    }

    vector<Coordinate> path;
    path.push_back(goal);
    while(current != nullptr) {
        path.push_back(current->coordinate);
        current = current->parent;
    }
    path.push_back(start);
    reverse(path.begin(), path.end());

    releaseNodes(openList);
    releaseNodes(closeList);

    return path;
}

vector<Coordinate> PathPlanning::generateBezier() {
    int num_nodes = path.size();
    int dimension = 2;
    double nodes[num_nodes*dimension];
    for (int i = 0; i < num_nodes; i++) {
        nodes[i*2] = path[i].x;
        nodes[i*2+1] = path[i].y;
    }
    int num_vals = 101;
    double s_vals[num_vals];
    for (int i = 0; i < num_vals; i++) {
        s_vals[i] = 0.01 * i;
    }
    double evaluated[num_vals*dimension];
    BEZ_evaluate_multi(&num_nodes, &dimension, nodes, &num_vals, s_vals, evaluated);

    vector<Coordinate> result;
    for (int i = 0; i < num_vals; i++) {
        result.push_back(Coordinate{(int)evaluated[i*2], (int)evaluated[i*2+1]});
    }

    return result;
}