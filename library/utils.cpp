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

GlobalData* GlobalData::m_UniqueInstance = new GlobalData();

GlobalData::GlobalData() {
    try {
        ifstream global_file(global_filename);
        ifstream position_file(position_filename);

        global = json::parse(global_file);
        position = json::parse(position_file);

        global_file.close();
        position_file.close();

        screen_height = global["screen_height"].template get<double>();
        screen_width = global["screen_width"].template get<double>();
        screen_padding = global["screen_padding"].template get<double>();
        robot_radius = global["robot_radius"].template get<double>();
        node_distance = global["node_distance"].template get<double>();

        heuristic_type = global["heuristic_type"].template get<int>();
        path_number = global["path_number"].template get<int>();
        bezier_curvature = global["bezier_curvature"].template get<int>();
        updateValue();
    } catch(...) {
        cout << "failed load/read json file" << endl;
        exit(-1);
    }
}

void GlobalData::updateValue() {
  enemies.clear();
  obstacles.clear();
  obstacles_visible.clear();

  auto convert_point_json = [](json point) {
      double x = point["x"].template get<double>();
      double y = point["y"].template get<double>();
      return Vec((x + 4.5) * 100, (y + 3) * 100);
  };

  robot = convert_point_json(position[path_number]["robot"]);
  ball = convert_point_json(position[path_number]["ball"]);
  for (auto &enemy : position[path_number]["enemies"]) {
      enemies.push_back(convert_point_json(enemy));
  }

  auto point_in_field = [&](Vec point) {
      if (
          point.x >= 0 && point.x <= screen_width &&
          point.y >= 0 && point.y <= screen_height
      ) return true;
      return false;
  };

  for (auto &enemy : enemies) {
      vector<Vec> obstacle_of_enemy, obstacle_of_enemy_visible;
      Vec temp_point{
          static_cast<int>(enemy.x / node_distance) * node_distance,
          static_cast<int>(enemy.y / node_distance) * node_distance
      };
      int offset = static_cast<int>(robot_radius / node_distance + 1);

      for (int i = -offset; i <= offset; i++) {
          for (int j = -offset; j <= offset; j++) {
              Vec neighbor = temp_point + Vec{i * node_distance, j * node_distance};
              Vec delta = enemy - neighbor;
              if (point_in_field(neighbor)) {
                if (delta.len() <= robot_radius)
                  obstacle_of_enemy.push_back(neighbor);
                if (delta.len() <= robot_radius/2)
                  obstacle_of_enemy_visible.push_back(neighbor);
              }
          }
      }
      obstacles.push_back(obstacle_of_enemy);
      obstacles_visible.push_back(obstacle_of_enemy_visible);
  }
}

double smoothValue(double value, double min_input, double max_input, double min_output, double max_output) {
  double clamp_value = value;
  if (value < min_input) clamp_value = min_input;
  if (value > max_input) clamp_value = max_input;
  return min_output + (clamp_value-min_input)*(max_output-min_output)/(max_input-min_input);
}