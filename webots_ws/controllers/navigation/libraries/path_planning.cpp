#include "path_planning.hpp"
#include "constants.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits.h>
#include <fstream>
#include <sstream>

bool Coordinate::operator==(const Coordinate& coord_) {
    return (x == coord_.x && y == coord_.y);
}

Coordinate Coordinate::operator+(const Coordinate& coord_) {
    return {x + coord_.x, y + coord_.y};
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
    vector<vector<double>> points;
    fstream file(Constants::positionFile, ios::in);
    string line, x, y;
    if (file.is_open()) {
        while (getline(file, line)) {
            stringstream str(line);
            getline(str, x, ',');
            getline(str, y, ',');
            points.push_back(vector<double>{stod(x), stod(y)});
        }
    }
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
    bezier_path = generateBezier(100);
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

float PathPlanning::heuristic(Coordinate& source_, Coordinate& target_, int type) {
    switch (type) {
    case 1: // manhatan
        return static_cast<float>(abs(target_.x-source_.x) + abs(target_.y-source_.y));
        break;
    case 2: // diagonal
        return static_cast<float>(max(abs(target_.x-source_.x), abs(target_.y-source_.y)));
        break;
    case 3: // octile
        return static_cast<float>(
            max(abs(target_.x-source_.x), abs(target_.y-source_.y)) +
            (sqrt(2)-1) * min(abs(target_.x-source_.x), abs(target_.y-source_.y))
        );
        break;
    default: // euclidean
        return static_cast<float>(sqrt(pow(target_.x-source_.x, 2) + pow(target_.y-source_.y, 2)));
        break;
    }
}

bool PathPlanning::detectCollision(Coordinate& coord_) {
    if (coord_.x < 0 || coord_.x >= Constants::WIDTH ||
        coord_.y < 0 || coord_.y >= Constants::HEIGHT ||
        std::find(walls.begin(), walls.end(), coord_) != walls.end()) {
        return true;
    }
    return false;
}

vector<Coordinate> PathPlanning::getNeighbors(Coordinate& coord_) {
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
    vector<Coordinate> directions = {
        { 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 },
        // { -1, -1 }, { 1, 1 }, { -1, 1 }, { 1, -1 }
    };
    for (auto &item : directions) {
        item.x = coord_.x + item.x * Constants::NODE_DISTANCE;
        item.y = coord_.y + item.y * Constants::NODE_DISTANCE;
    }
    if (coord_ == start) {
        return start_area;
    } else if (std::find(start_area.begin(), start_area.end(), coord_) != start_area.end()) {
        directions.push_back(start);
    } else if (std::find(goal_area.begin(), goal_area.end(), coord_) != goal_area.end()) {
        directions.push_back(goal);
    }
    return directions;
    
}

Node* PathPlanning::findNodeOnList(vector<Node*>& nodes_, Coordinate& coordinate_) {
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
    openList.push_back(new Node(start));
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

        if (current->coordinate == goal) break;

        closeList.push_back(current);
        openList.erase(current_it);

        for (Coordinate& neighbor : getNeighbors(current->coordinate)) {
            if (detectCollision(neighbor) || findNodeOnList(closeList, neighbor)) continue;
            int totalCost = current->G + heuristic(current->coordinate, neighbor, 4);

            Node* successor = findNodeOnList(openList, neighbor);
            if (successor == nullptr) {
                successor = new Node(neighbor, current);
                successor->G = totalCost;
                successor->H = heuristic(successor->coordinate, goal, 4);
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

vector<Coordinate> PathPlanning::generateBezier(int numPoints) {
    vector<Coordinate> result(numPoints+1);
    for (int i = 0; i <= numPoints; ++i) {
        double t = static_cast<double>(i) / numPoints;
        Coordinate p = calculateBezierPoint(t);
        result[i] = p;
    }
    return result;
}

Coordinate PathPlanning::calculateBezierPoint(double t) {
    vector<pair<double, double>> points;
    for (auto &point : path) {
        points.push_back(pair<double, double>{static_cast<double>(point.x), static_cast<double>(point.y)});
    }
    int n = points.size() - 1;

    for (int r = 1; r <= n; ++r) {
        for (int i = 0; i <= n - r; ++i) {
            points[i].first = (1 - t) * points[i].first + t * points[i + 1].first;
            points[i].second = (1 - t) * points[i].second + t * points[i + 1].second;
        }
    }

    return Coordinate{static_cast<int>(points[0].first), static_cast<int>(points[0].second)};
}