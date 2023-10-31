#include "path_generator.hpp"

Node::Node(Vec coordinate, Node* parent) {
    this->parent = parent;
    this->coordinate = coordinate;
    G = H = 0;
}

double Node::getScore(){
    return G + H; 
}

double PathGenerator::getAstarLength() {
  double distance = 0;
  for (size_t i = 0; i < global->astar_path.size()-1; i++) {
    distance += (global->astar_path[i] - global->astar_path[i+1]).len();
  }
  return distance;
}

double PathGenerator::getBezierLength() {
  double distance = 0;
  for (size_t i = 0; i < global->bezier_path.size()-1; i++) {
    distance += (global->bezier_path[i] - global->bezier_path[i+1]).len();
  }
  return distance;
}

double PathGenerator::heuristic(Vec source, Vec target, int type) {
    switch (type) {
    case 1: // manhatan
        return abs(target.x-source.x) + abs(target.y-source.y);
        break;
    case 2: // chebyshev
        return max(abs(target.x-source.x), abs(target.y-source.y));
        break;
    case 3: // octile
        return abs(target.x-source.x) + abs(target.y-source.y) +
            (sqrt(2)-2) * min(abs(target.x-source.x), abs(target.y-source.y));
        break;
    case 4: // euclidean
        return sqrt(pow(target.x-source.x, 2) + pow(target.y-source.y, 2));
        break;
    default:
      cout << "false heuristic method" << endl;
      return 0;
    }
}

bool PathGenerator::detectCollision(Vec pos) {
    if (pos.x <= 0 || pos.x >= global->screen_width ||
        pos.y <= 0 || pos.y >= global->screen_height) {
        return true;
    }
    for (auto &item : global->obstacles) {
        for (auto &obstacle : item) {
            if (obstacle == pos) {
                return true;
            }
        }
    }
    return false;
}

vector<Vec> PathGenerator::getNeighbors(Vec pos, bool ignore_head) {
  vector<Vec> directions = {
      { 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 },
      { -1, -1 }, { 1, 1 }, { -1, 1 }, { 1, -1 }
  };
  for (auto &item : directions) {
      item = pos + item * global->node_distance;
  }
  if (ignore_head) return directions;
  double temp_x = static_cast<int>(global->robot.x / global->node_distance) * global->node_distance;
  double temp_y = static_cast<int>(global->robot.y / global->node_distance) * global->node_distance;
  vector<Vec> start_area {
      Vec{temp_x, temp_y},
      Vec{temp_x+global->node_distance, temp_y},
      Vec{temp_x, temp_y+global->node_distance},
      Vec{temp_x+global->node_distance, temp_y+global->node_distance},
  };
  temp_x = (int)(global->ball.x / global->node_distance) * global->node_distance;
  temp_y = (int)(global->ball.y / global->node_distance) * global->node_distance;
  vector<Vec> goal_area {
      Vec{temp_x, temp_y},
      Vec{temp_x+global->node_distance, temp_y},
      Vec{temp_x, temp_y+global->node_distance},
      Vec{temp_x+global->node_distance, temp_y+global->node_distance},
  };
  if (pos == global->robot) {
      return start_area;
  } else if (std::find(start_area.begin(), start_area.end(), pos) != start_area.end()) {
      directions.push_back(global->robot);
  } else if (std::find(goal_area.begin(), goal_area.end(), pos) != goal_area.end()) {
      directions.push_back(global->ball);
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
    nodes.clear();
}

void PathGenerator::generatePath() {
  current = nullptr;
  global->visited_node.clear();
  
  openList.clear();
  closeList.clear();

  if ((global->robot - global->ball).len() < global->robot_radius) {
    global->astar_path = vector<Vec>{global->robot, global->ball};
    return;
  }

  bool isFound = false;
  openList.push_back(new Node(global->robot));
  while (!openList.empty()) {
    if (astar_find_next_node()) {
      isFound = true;
      break;
    }
    astar_find_neighbors();
  }

  if (!isFound) {
    global->visited_node.clear();
    global->astar_path = vector<Vec>{global->robot, global->ball};
    return;
  }

  process_path();
  modified_path();
}

bool PathGenerator::astar_find_next_node() {
  auto current_it = openList.begin();
  current = *current_it;
  for (auto it = openList.begin(); it != openList.end(); it++) {
      auto node = *it;
      if (node->getScore() < current->getScore()) {
          current = node;
          current_it = it;
      }
  }
  closeList.push_back(current);
  openList.erase(current_it);

  if (current->coordinate == global->ball) return true;
  return false;
}

void PathGenerator::astar_find_neighbors(bool ignore_head) {
  for (auto &neighbor : getNeighbors(current->coordinate, ignore_head)) {
      if (detectCollision(neighbor) || findNodeOnList(closeList, neighbor)) continue;
      int totalCost = current->G + (current->coordinate - neighbor).len();

      Node* successor = findNodeOnList(openList, neighbor);
      if (successor == nullptr) {
          successor = new Node(neighbor, current);
          successor->G = totalCost;
          successor->H = heuristic(successor->coordinate, global->ball, global->heuristic_type);
          openList.push_back(successor);
          global->visited_node.push_back(neighbor);
      } else if (totalCost < successor->G) {
          successor->parent = current;
          successor->G = totalCost;
      }
  }
}

void PathGenerator::process_path() {
  vector<Vec> path;
  path.push_back(global->ball);
  while(current != nullptr) {
      path.push_back(current->coordinate);
      current = current->parent;
  }
  path.push_back(global->robot);
  reverse(path.begin(), path.end());

  releaseNodes(openList);
  releaseNodes(closeList);

  global->astar_path = path;
}

void PathGenerator::modified_path(bool ignore_head) {
  if (!ignore_head && global->astar_path.size() < 5) return;
  if (ignore_head && global->astar_path.size() < 3) return;

  auto get_dir_func = [](Vec point1, Vec point2) {
    Vec delta = point1-point2;
    if (delta.x > 0 && delta.y == 0) return 1;
    if (delta.x > 0 && delta.y > 0) return 2;
    if (delta.x > 0 && delta.y < 0) return 3;
    if (delta.x < 0 && delta.y == 0) return 4;
    if (delta.x < 0 && delta.y > 0) return 5;
    if (delta.x < 0 && delta.y < 0) return 6;
    if (delta.x == 0 && delta.y > 0) return 7;
    if (delta.x == 0 && delta.y < 0)return 8;
    return -1;
  };

  vector<Vec> filter_path;
  filter_path.push_back(global->astar_path[0]);
  size_t start_index;
  if (!ignore_head) {
    start_index = 2;
    filter_path.push_back(global->astar_path[1]);
  } else {
    start_index = 1;
  }
  size_t i = start_index;
  int dir = get_dir_func(global->astar_path[start_index-1], global->astar_path[start_index]);
  while (i != global->astar_path.size()-start_index) {
    int new_dir = get_dir_func(global->astar_path[i], global->astar_path[i+1]);
    if (dir != new_dir) {
      for (int j = 0; j < global->bezier_curvature; j++) filter_path.push_back(global->astar_path[i]);
      dir = new_dir;
    } else {
      filter_path.push_back(global->astar_path[i]);
    }
    i++;
  }
  if (!ignore_head) {
    filter_path.push_back(global->astar_path[global->astar_path.size()-2]);
  }
  filter_path.push_back(global->astar_path[global->astar_path.size()-1]);

  if (!ignore_head) global->astar_path = filter_path;
  else global->modified_astar_path = filter_path;
}

void PathGenerator::generateSmoothPath(int numPoints) {
    vector<Vec> result(numPoints+1);
    for (int i = 0; i <= numPoints; ++i) {
        double t = static_cast<double>(i) / numPoints;
        vector<Vec> points;
        for (Vec point : global->astar_path) {
            points.push_back(point);
        }
        int n = points.size();

        for (int j = 1; j < n; ++j) {
            for (int k = 0; k < n - j; ++k) {
                points[k] = points[k]*(1 - t) + points[k + 1]*t;
            }
        }
        result[i] = points[0];
    }
    global->bezier_path = result;
}

void PathGenerator::getBezierPoints(int numPoints, int index) {
  vector<Vec> result(index+1);
  vector<vector<Vec>> control_points;
  for (int i = 0; i <= index; ++i) {
      double t = static_cast<double>(i) / numPoints;
      vector<Vec> points;
      for (Vec point : global->modified_astar_path) {
          points.push_back(point);
      }
      int n = points.size();
      
      for (int j = 1; j < n; ++j) {
        vector<Vec> temp;
        for (int k = 0; k < n - j; ++k) {
            points[k] = points[k]*(1 - t) + points[k + 1]*t;
            temp.push_back(points[k]);
        }
        control_points.push_back(temp);
      }
      result[i] = points[0];
  }
  global->bezier_path = result;
  global->control_points = control_points;
}