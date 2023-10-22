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

Vec convertPoint(json point) {
  double x = point["x"].template get<double>();
  double y = point["y"].template get<double>();
  return Vec((x + 4.5) * 100, (y + 3) * 100);
}

json convertPoint(Vec point) {
  json data;
  data["x"] = point.x / 100 - 4.5;
  data["y"] = point.y / 100 - 3;
  return data;
}

GlobalData::GlobalData(string dir_) {
  dir = dir_;
  global_filename = dir + "config/global.json";
  position_filename = dir + "config/position.json";
  worlds_filename = dir + "webots_ws/worlds/soccer.wbt";
  try {
    loadFile();
    updateGlobal();
    updatePosition();
    updateTargetPosition();
    updateObstacles();
  } catch(...) {
    cerr << "failed to initialize global data" << endl;
  }
}

void GlobalData::loadFile() {
    ifstream global_file(global_filename);
    ifstream position_file(position_filename);

    global = json::parse(global_file);
    position = json::parse(position_file);

    global_file.close();
    position_file.close();
}

void GlobalData::updateGlobal() {
    screen_height = global["screen_height"].template get<double>();
    screen_width = global["screen_width"].template get<double>();
    screen_padding = global["screen_padding"].template get<double>();
    robot_radius = global["robot_radius"].template get<double>();
    node_distance = global["node_distance"].template get<double>();
    heuristic_type = global["heuristic_type"].template get<int>();
    path_number = global["path_number"].template get<int>();
    bezier_curvature = global["bezier_curvature"].template get<int>();
}

void GlobalData::updatePosition() {
  enemies.clear();
  
  robot = convertPoint(position[path_number]["robot"]);
  ball = convertPoint(position[path_number]["ball"]);
  for (auto &enemy : position[path_number]["enemies"]) {
      enemies.push_back(convertPoint(enemy));
  }
}

void GlobalData::updateObstacles() {
  obstacles.clear();
  obstacles_visible.clear();

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

void GlobalData::updateTargetPosition() {
  target_position.clear();

  for (auto &data : position[path_number]["target"]) {
    vector<Vec> temp;
    for (auto &position : data) {
      temp.push_back(convertPoint(position));
    }
    target_position.push_back(temp);
  }
}

void GlobalData::saveValue() {
  global["path_number"] = path_number;
  global["heuristic_type"] = heuristic_type;
  global["robot_radius"] = robot_radius;
  global["node_distance"] = node_distance;
  global["bezier_curvature"] = bezier_curvature;
  
  position[path_number]["robot"] = convertPoint(robot);
  position[path_number]["ball"] = convertPoint(ball);
  json enemies_data;
  for (auto enemy : enemies) {
    enemies_data.push_back(convertPoint(enemy));
  }
  position[path_number]["enemies"] = enemies_data;

  json target_position_json;
  for (auto &data : target_position) {
    json temp;
    for (auto &position : data) {
      temp.push_back(convertPoint(position));
    }
    target_position_json.push_back(temp);
  }
  position[path_number]["target"] = target_position_json;

  ofstream global_file(global_filename);
  ofstream position_file(position_filename);
  
  global_file << global;
  position_file << position;
  
  global_file.close();
  position_file.close();

  ifstream world_file_input(worlds_filename);
  vector<string> lines;
  string line;
  while (getline(world_file_input, line)) {
      lines.push_back(line);
  }
  world_file_input.close();
  vector<int> update_line{31, 35, 48, 62, 76, 90, 104};
  auto update_string_position = [&](string pos_str, Vec pos) {
      stringstream ss(pos_str);
      vector<string> nums;
      string num;

      while (ss >> num) {
          nums.push_back(num);
      }

      double x_ = pos.x / 100 - 4.5;
      double y_ = pos.y / 100 - 3;
      
      stringstream result;
      result << "  " << nums[0] << " " << x_ << " " << y_ << " " << nums[3];
      return result.str();
  };

  lines[update_line[0]] = update_string_position(lines[update_line[0]], ball);
  lines[update_line[1]] = update_string_position(lines[update_line[1]], robot);
  for (size_t i = 2; i < update_line.size(); i++) {
      lines[update_line[i]] = update_string_position(lines[update_line[i]], enemies[i-2]);
  }

  ofstream world_file_output(worlds_filename);
  for (auto line : lines) {
      world_file_output << line << endl;
  }
  world_file_output.close();
}