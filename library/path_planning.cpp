#include "path_planning.hpp"

// Coordinate struct
bool Coordinate::operator==(Coordinate coord) {
    return (x == coord.x && y == coord.y);
}

Coordinate Coordinate::operator+(Coordinate coord) {
    return {x + coord.x, y + coord.y};
}

Coordinate Coordinate::operator-(Coordinate coord) {
    return {x - coord.x, y - coord.y};
}

Coordinate Coordinate::operator*(double scalar) {
    return {x * scalar, y * scalar};
}

double Coordinate::dot(Coordinate coord) {
    return x * coord.x + y * coord.y;
}

double Coordinate::len() {
    return sqrt(x * x + y * y);
}

ostream& operator<<(ostream& os, Coordinate coord) {
    return os << "{" << coord.x << "," << coord.y << "}";
}

// Node class
Node::Node(Coordinate coordinate_, Node* parent_) {
    parent = parent_;
    coordinate = coordinate_;
    G = H = 0;
}

double Node::getScore(){
    return G + H; 
}

PathPlanning::PathPlanning() {
    ifstream position_file("config/position.json");
    json data = json::parse(position_file);
    // initialize data
    start = transformPoint(Coordinate{
        data["robot"]["x"].template get<double>(),
        data["robot"]["y"].template get<double>()
    });
    goal = transformPoint(Coordinate{
        data["ball"]["x"].template get<double>(),
        data["ball"]["y"].template get<double>()
    });
    for (auto &item : data["enemy"]) {
        enemy.push_back(transformPoint(Coordinate{
            item["x"].template get<double>(),
            item["y"].template get<double>()
        }));
    }

    ifstream global_file("config/global.json");
    json global = json::parse(global_file);
    HEIGHT = global["screen_height"].template get<double>();
    WIDTH = global["screen_width"].template get<double>();
    NODE_DISTANCE = global["node_distance"].template get<double>();
    ENEMY_RADIUS = global["enemy_radius"].template get<double>();

    for (int x = NODE_DISTANCE; x < WIDTH; x += NODE_DISTANCE) {
      for (int y = NODE_DISTANCE; y < HEIGHT; y += NODE_DISTANCE) {
        for (auto item : enemy) {
          double dis = std::sqrt(pow(x - item.x, 2) + pow(y - item.y, 2));
          if (dis < ENEMY_RADIUS){
            walls.push_back(Coordinate{static_cast<double>(x), static_cast<double>(y)});
          }
        }
      }
    }

    path = findPath();
    bezier_path = generateBezier(100);
}

pair<double, double> PathPlanning::getStart() {
    return pair<double, double>{start.x, start.y};
}

pair<double, double> PathPlanning::getGoal() {
    return pair<double, double>{goal.x, goal.y};
}

vector<pair<double, double>> PathPlanning::getEnemy() {
    vector<pair<double, double>> result;
    for (auto &item : enemy) {
        result.push_back(pair<double, double>{item.x, item.y});
    }
    return result;
}

vector<pair<double, double>> PathPlanning::getCollision() {
    vector<pair<double, double>> result;
    for (auto &item : walls) {
        result.push_back(pair<double, double>{item.x, item.y});
    }
    return result;
}

vector<pair<double, double>> PathPlanning::getPath() {
    vector<pair<double, double>> result;
    for (auto &item : path) {
        result.push_back(pair<double, double>{item.x, item.y});
    }
    return result;
}

vector<pair<double, double>> PathPlanning::getBezierPath() {
    vector<pair<double, double>> result;
    for (auto &item : bezier_path) {
        result.push_back(pair<double, double>{item.x, item.y});
    }
    return result;
}

Coordinate PathPlanning::transformPoint(Coordinate coord) {
    double x = (coord.x + 4.5) * 100;
    double y = (coord.y + 3) * 100;
    return Coordinate{x, y};
}

double PathPlanning::heuristic(Coordinate source, Coordinate target, int type) {
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

bool PathPlanning::detectCollision(Coordinate coord) {
    if (coord.x < 0 || coord.x >= WIDTH ||
        coord.y < 0 || coord.y >= HEIGHT ||
        std::find(walls.begin(), walls.end(), coord) != walls.end()) {
        return true;
    }
    return false;
}

vector<Coordinate> PathPlanning::getNeighbors(Coordinate coord) {
    double temp_x = (int)(start.x / NODE_DISTANCE) * NODE_DISTANCE;
    double temp_y = (int)(start.y / NODE_DISTANCE) * NODE_DISTANCE;
    vector<Coordinate> start_area {
        Coordinate{temp_x, temp_y},
        Coordinate{temp_x+NODE_DISTANCE, temp_y},
        Coordinate{temp_x, temp_y+NODE_DISTANCE},
        Coordinate{temp_x+NODE_DISTANCE, temp_y+NODE_DISTANCE},
    };
    temp_x = (int)(goal.x / NODE_DISTANCE) * NODE_DISTANCE;
    temp_y = (int)(goal.y / NODE_DISTANCE) * NODE_DISTANCE;
    vector<Coordinate> goal_area {
        Coordinate{temp_x, temp_y},
        Coordinate{temp_x+NODE_DISTANCE, temp_y},
        Coordinate{temp_x, temp_y+NODE_DISTANCE},
        Coordinate{temp_x+NODE_DISTANCE, temp_y+NODE_DISTANCE},
    };
    vector<Coordinate> directions = {
        { 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 },
        { -1, -1 }, { 1, 1 }, { -1, 1 }, { 1, -1 }
    };
    for (auto &item : directions) {
        item.x = coord.x + item.x * NODE_DISTANCE;
        item.y = coord.y + item.y * NODE_DISTANCE;
    }
    if (coord == start) {
        return start_area;
    } else if (std::find(start_area.begin(), start_area.end(), coord) != start_area.end()) {
        directions.push_back(start);
    } else if (std::find(goal_area.begin(), goal_area.end(), coord) != goal_area.end()) {
        directions.push_back(goal);
    }
    return directions;
    
}

Node* PathPlanning::findNodeOnList(vector<Node*>& nodes, Coordinate coordinate) {
    for (auto node : nodes) {
        if (node->coordinate == coordinate) {
            return node;
        }
    }
    return nullptr;
}

void PathPlanning::releaseNodes(vector<Node*>& nodes) {
    for (int i = 0; i < nodes.size(); i++)
        delete nodes[i];
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
        points.push_back(pair<double, double>{point.x, point.y});
    }
    int n = points.size() - 1;

    for (int r = 1; r <= n; ++r) {
        for (int i = 0; i <= n - r; ++i) {
            points[i].first = (1 - t) * points[i].first + t * points[i + 1].first;
            points[i].second = (1 - t) * points[i].second + t * points[i + 1].second;
        }
    }

    return Coordinate{points[0].first, points[0].second};
}