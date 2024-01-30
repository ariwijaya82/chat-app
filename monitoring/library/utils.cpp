#include "utils.hpp"

param loadParameter() {
    string path = "data/parameter.json";
    ifstream file(path);
    json data = json::parse(file);
    file.close();

    param p;
    p.node_distance = data["node_distance"].template get<int>();
    p.heuristic_type = data["heuristic_type"].template get<int>();
    p.bezier_curvature = data["bezier_curvature"].template get<int>();
    p.robot_radius = data["robot_radius"].template get<int>();
    p.path_number = data["path_number"].template get<int>();
    p.screen_width = data["screen_width"].template get<int>();
    p.screen_height = data["screen_height"].template get<int>();
    p.screen_padding = data["screen_padding"].template get<int>();
    
    return p;
}

position loadPosition(int path_number) {
    string path = "data/position.json";
    ifstream file(path);
    json data = json::parse(file);
    file.close();

    position p;
    p.ball = convertJSONtoPoint(data[path_number]["ball"]);
    p.robot = convertJSONtoPoint(data[path_number]["robot"]);
    json enemies = data[path_number]["enemies"];
    json target = data[path_number]["target"];
    for (json enemy : enemies) {
        p.enemies.push_back(convertJSONtoPoint(enemy));
    }
    for (json enemy : target) {
        p.target.push_back(vector<Point>());
        for (json pos : enemy) {
            p.target.back().push_back(convertJSONtoPoint(pos));
        }
    }

    return p;
}

Point convertJSONtoPoint(json pos) {
    double x = pos["x"].template get<double>();
    double y = pos["y"].template get<double>();
    return Point(x,y);
}

vector<Point> getPointAround(Point p, int distance, int radius) {
    vector<Point> result;
    Point round_point{
        (double)((int)p.x / distance),
        (double)((int)p.y / distance)
    };
    int offset = radius / distance + 1;
    for (int i = -offset; i <= offset; i++) {
        for (int j = -offset; j <= offset; j++) {
            Point other = round_point + Point{(double)(i * distance), (double)(j * distance)};
            Point delta = p - other;
            if (delta.len() <= radius) result.push_back(other);
        }
    }
    return result;
}

QPointF convertCartesianToQt(Point p) {
    param par = loadParameter();
    return QPointF(p.x + par.screen_padding, par.screen_height + par.screen_padding - p.y);
}

Point convertQtToCartesian(QPointF p) {
    param par = loadParameter();
    return Point(p.x() - par.screen_padding, par.screen_height + par.screen_padding - p.y());
}