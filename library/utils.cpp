#include "utils.hpp"

Vec::Vec(double x, double y) {
    this->x = x;
    this->y = y;
}

double Vec::len() {
    return sqrt(x*x + y*y);
}

Vec Vec::operator+(Vec vec) {
    return Vec(x + vec.x, y + vec.y);
}

Vec Vec::operator-(Vec vec) {
    return Vec(x - vec.x, y - vec.y);
}

Vec Vec::operator*(double scalar) {
    return Vec(x * scalar, y * scalar);
}

Vec Vec::operator/(double scalar) {
    return Vec(x / scalar, y / scalar);
}

bool Vec::operator==(Vec vec) {
    return x == vec.x && y == vec.y;
}

ostream& operator<<(ostream &os, Vec vec) {
    return os << "{" << vec.x << "," << vec.y << "}";
}

Constant::Constant() {
    try {
        ifstream global_file("config/global.json");
        ifstream position_file("config/position.json");

        json global = json::parse(global_file);
        json position = json::parse(position_file);

        height = global["screen_height"].template get<double>();
        width = global["screen_width"].template get<double>();
        padding = global["screen_padding"].template get<double>();
        radius = global["enemy_radius"].template get<double>();
        distance = global["node_distance"].template get<double>();

        heuristic_type = global["heuristic_type"].template get<int>();
        range_x = global["range_x"].template get<double>();
        range_a = global["range_a"].template get<double>();
    
        auto convert_point_json = [](json point) {
            double x = point["x"].template get<double>();
            double y = point["y"].template get<double>();
            return Vec((x + 4.5) * 100, (y + 3) * 100);
        };

        robot = convert_point_json(position["robot"]);
        ball = convert_point_json(position["ball"]);
        for (auto &enemy : position["enemies"]) {
            enemies.push_back(convert_point_json(enemy));
        }
        // cout << "(" << (int)robot.x << "," << (int)robot.y << ")" << endl
        //      << "(" << (int)enemies[0].x << "," << (int)enemies[0].y << ")" << endl
        //      << "(" << (int)enemies[1].x << "," << (int)enemies[1].y << ")" << endl
        //      << "(" << (int)enemies[2].x << "," << (int)enemies[2].y << ")" << endl
        //      << "(" << (int)enemies[3].x << "," << (int)enemies[3].y << ")" << endl
        //      << "(" << (int)enemies[4].x << "," << (int)enemies[4].y << ")" << endl
        //      << "(" << (int)ball.x << "," << (int)ball.y << ")" << endl << endl;

        auto point_in_field = [&](Vec point) {
            if (
                point.x >= 0 && point.x <= width &&
                point.y >= 0 && point.y <= height
            ) return true;
            return false;
        };

        for (auto &enemy : enemies) {
            vector<Vec> obstacle_of_enemy;
            Vec temp_point{
                static_cast<int>(enemy.x / distance) * distance,
                static_cast<int>(enemy.y / distance) * distance
            };
            int offset = static_cast<int>(radius / distance + 1);

            for (int i = -offset; i <= offset; i++) {
                for (int j = -offset; j <= offset; j++) {
                    Vec neighbor = temp_point + Vec{i * distance, j * distance};
                    Vec delta = enemy - neighbor;
                    if (
                        point_in_field(neighbor) &&
                        delta.len() <= radius
                    ) {
                        obstacle_of_enemy.push_back(neighbor);
                    }
                }
            }
            obstacles.push_back(obstacle_of_enemy);
        }
    } catch(...) {
        cout << "failed load/read json file" << endl;
    }
}
