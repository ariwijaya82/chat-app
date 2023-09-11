#include "path_generator.hpp"

// Node struct
Node::Node(Vec coordinate, Node* parent) {
    this->parent = parent;
    this->coordinate = coordinate;
    G = H = 0;
}

double Node::getScore(){
    return G + H; 
}

// Path Generator
PathGenerator::PathGenerator() {
    constant = new Constant();
    HEIGHT = constant->height;
    WIDTH = constant->width;
    NODE_DISTANCE = constant->distance;
    
    robot = constant->robot;
    ball = constant->ball;
    enemies = constant->enemies;
    obstacles = constant->obstacles;
    
    generatePath();
    generateSmoothPath(100);
}

PathGenerator::~PathGenerator() {
    delete constant;
}

double PathGenerator::heuristic(Vec source, Vec target, int type) {
    switch (type) {
    case 1: // manhatan
        return abs(target.x-source.x) + abs(target.y-source.y);
        break;
    case 2: // diagonal
        return max(abs(target.x-source.x), abs(target.y-source.y));
        break;
    case 3: // octile
        return max(abs(target.x-source.x), abs(target.y-source.y)) +
            (sqrt(2)-1) * min(abs(target.x-source.x), abs(target.y-source.y));
        break;
    default: // euclidean
        return sqrt(pow(target.x-source.x, 2) + pow(target.y-source.y, 2));
        break;
    }
}

bool PathGenerator::detectCollision(Vec pos) {
    if (pos.x <= 0 || pos.x >= WIDTH ||
        pos.y <= 0 || pos.y >= HEIGHT) {
        return true;
    }
    for (auto &item : obstacles) {
        for (auto &obstacle : item) {
            if (obstacle == pos) {
                return true;
            }
        }
    }
    return false;
}

vector<Vec> PathGenerator::getNeighbors(Vec pos) {
    double temp_x = static_cast<int>(robot.x / NODE_DISTANCE) * NODE_DISTANCE;
    double temp_y = static_cast<int>(robot.y / NODE_DISTANCE) * NODE_DISTANCE;
    vector<Vec> start_area {
        Vec{temp_x, temp_y},
        Vec{temp_x+NODE_DISTANCE, temp_y},
        Vec{temp_x, temp_y+NODE_DISTANCE},
        Vec{temp_x+NODE_DISTANCE, temp_y+NODE_DISTANCE},
    };
    temp_x = (int)(ball.x / NODE_DISTANCE) * NODE_DISTANCE;
    temp_y = (int)(ball.y / NODE_DISTANCE) * NODE_DISTANCE;
    vector<Vec> goal_area {
        Vec{temp_x, temp_y},
        Vec{temp_x+NODE_DISTANCE, temp_y},
        Vec{temp_x, temp_y+NODE_DISTANCE},
        Vec{temp_x+NODE_DISTANCE, temp_y+NODE_DISTANCE},
    };
    vector<Vec> directions = {
        { 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 },
        { -1, -1 }, { 1, 1 }, { -1, 1 }, { 1, -1 }
    };
    for (auto &item : directions) {
        item.x = pos.x + item.x * NODE_DISTANCE;
        item.y = pos.y + item.y * NODE_DISTANCE;
    }
    if (pos == robot) {
        return start_area;
    } else if (std::find(start_area.begin(), start_area.end(), pos) != start_area.end()) {
        directions.push_back(robot);
    } else if (std::find(goal_area.begin(), goal_area.end(), pos) != goal_area.end()) {
        directions.push_back(ball);
    }
    return directions;
    
}

Node* PathGenerator::findNodeOnList(vector<Node*>& nodes, Vec coordinate) {
    for (auto node : nodes) {
        if (node->coordinate == coordinate) {
            return node;
        }
    }
    return nullptr;
}

void PathGenerator::releaseNodes(vector<Node*>& nodes) {
    for (size_t i = 0; i < nodes.size(); i++)
        delete nodes[i];
}

void PathGenerator::generatePath() {
    Node* current = nullptr;
    vector<Node*> openList, closeList;
    
    openList.reserve(100);
    closeList.reserve(100);
    openList.push_back(new Node(robot));
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

        if (current->coordinate == ball) break;

        closeList.push_back(current);
        openList.erase(current_it);

        for (auto &neighbor : getNeighbors(current->coordinate)) {
            if (detectCollision(neighbor) || findNodeOnList(closeList, neighbor)) continue;
            int totalCost = current->G + heuristic(current->coordinate, neighbor, 4);

            Node* successor = findNodeOnList(openList, neighbor);
            if (successor == nullptr) {
                successor = new Node(neighbor, current);
                successor->G = totalCost;
                successor->H = heuristic(successor->coordinate, ball, 4);
                openList.push_back(successor);
            } else if (totalCost < successor->G) {
                successor->parent = current;
                successor->G = totalCost;
            }
        }
    }

    vector<Vec> path;
    path.push_back(ball);
    while(current != nullptr) {
        path.push_back(current->coordinate);
        current = current->parent;
    }
    path.push_back(robot);
    reverse(path.begin(), path.end());

    releaseNodes(openList);
    releaseNodes(closeList);

    astar_path = path;
}

void PathGenerator::generateSmoothPath(int numPoints) {
    vector<Vec> result(numPoints+1);
    for (int i = 0; i <= numPoints; ++i) {
        double t = static_cast<double>(i) / numPoints;
        vector<pair<double, double>> points;
        for (auto &point : astar_path) {
            points.push_back(pair<double, double>{point.x, point.y});
        }
        int n = points.size() - 1;

        for (int r = 1; r <= n; ++r) {
            for (int i = 0; i <= n - r; ++i) {
                points[i].first = (1 - t) * points[i].first + t * points[i + 1].first;
                points[i].second = (1 - t) * points[i].second + t * points[i + 1].second;
            }
        }
        Vec p{points[0].first, points[0].second};
        result[i] = p;
    }
    bezier_path = result;
}