#include "plot.hpp"
#include "walk.hpp"
#include "astar.hpp"
#include "bezier.h"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>

void read_position_csv(std::vector<double> &x_path, std::vector<double> &y_path) {
  std::string filename = "../../data/position.csv";
  std::fstream file(filename, std::ios::in);
  std::string line, value;
  if (file.is_open()) {
    while (getline(file, line)) {
      std::stringstream str(line);
      getline(str, value, ',');
      x_path.push_back(stod(value));
      getline(str, value, ',');
      y_path.push_back(stod(value));
    }
  }
}

void read_path(std::vector<std::pair<int,int>> &path) {
  std::string filename = "../../data/curve.csv";
  std::fstream file(filename, std::ios::in);
  std::string line, value;
  if (file.is_open()) {
    while (getline(file, line)) {
      std::stringstream str(line);
      getline(str, value, ',');
      int x = stod(value);
      getline(str, value, ',');
      int y = stod(value);
      path.push_back(std::pair<int,int>{x, y});
    }
  }
}

void write_path_astar(std::vector<std::pair<int,int>>& path) {
  std::string filename = "../../data/path.csv";
  std::fstream file(filename, std::ios::out);
  for (auto item : path) {
    file << item.first << "," << item.second << std::endl;
  }
}

double computeBinominal(int n, int k){
    double value = 1.0;
    for (int i = 1; i <= k; i++) {
        value = value * ((n + 1 - i) / i);
    }
    if (n == k) {
        value = 1;
    }
    return value;
}

std::vector<std::pair<int,int>> computeNVertexBasierCurve2D(std::vector<std::pair<int,int>>& points) {
    std::vector<std::pair<int,int>> result;

    int n = points.size() - 1;
    for (double t = 0.0; (int)(t*100) <= 100; t += 0.01) {
        double bCurveXt{0};
        double bCurveYt{0};
        for (int i = 0; i <= n; ++i) {
            bCurveXt += computeBinominal(n, i) * std::pow((1 - t), (n - i)) * std::pow(t, i) * points[i].first;
            bCurveYt += computeBinominal(n, i) * std::pow((1 - t), (n - i)) * std::pow(t, i) * points[i].second;
        }
        result.push_back(std::pair{bCurveXt, bCurveYt});
    }
    return result;
}

double smoothValue(double value, double min_input, double max_input, double min_output, double max_output) {
  double clamp_value = value;
  if (value < min_input) clamp_value = min_input;
  if (value > max_input) clamp_value = max_input;
  return min_output + (clamp_value-min_input)*(max_output-min_output)/(max_input-min_input);
}

void test_bezier() {
  int num_nodes = 2;
  int dimension = 2;
  double nodes[4] = { 0.0, 0.0, 3.0, 4.0 };

  // Outputs.
  double length;
  int error_val;

  BEZ_compute_length(&num_nodes, &dimension, nodes, &length, &error_val);
  std::cout << "length: " << length << std::endl;
}

void calc_path(std::vector<std::pair<int,int>>& path, std::vector<std::pair<int,int>>& result) {
  int num_nodes = path.size();
  int dimension = 2;
  double nodes[num_nodes*dimension];
  for (int i = 0; i < num_nodes; i++) {
    nodes[i*2] = path[i].first;
    nodes[i*2+1] = path[i].second;
  }
  int num_vals = 101;
  double s_vals[num_vals];
  for (int i = 0; i < num_vals; i++) {
    s_vals[i] = 0.01 * i;
  }
  double evaluated[num_vals*dimension];
  BEZ_evaluate_multi(&num_nodes, &dimension, nodes, &num_vals, s_vals, evaluated);
  for (int i = 0; i < num_vals; i++) {
    result.push_back(std::pair<int,int>{evaluated[i*2], evaluated[i*2+1]});
  }
}

int main(int argc, char** argv) {
    QApplication* app = new QApplication(argc, argv);
    Plot* plot = new Plot();
    plot->show();

    Walk* walk = new Walk();

    int width = 900, height = 600, distance = 50;
    AStar::Generator* generator = new AStar::Generator(
      AStar::Vec2i{(width/distance)-1, (height/distance)-1});

    std::vector<double> x_path, y_path;
    read_position_csv(x_path, y_path);

    plot->setBallPosition(x_path[0]*100, y_path[0]*100);
    std::vector<std::pair<int, int>> enemy;
    for (int i = 2; i < x_path.size(); i++) {
      enemy.push_back(std::pair<int, int>{x_path[i]*100, y_path[i]*100});
    }
    plot->setEnemyPosition(enemy);

    std::vector<std::pair<int, int>> points;
    std::vector<std::pair<int, int>> collision;
    for (int i = 1; i < width/distance; i++) {
      for (int j = 1; j < height/distance; j++) {
        points.push_back(std::pair<int,int>{i*distance-450, j*distance-300});
        for (auto item : enemy) {
          double dis = std::sqrt(pow(i*distance-450 - item.first, 2) + pow(j*distance-300 - item.second, 2));
          if (dis < 50.0){
            collision.push_back(std::pair<int,int>{i*distance-450, j*distance-300});
            generator->addCollision(AStar::Vec2i{i, j});
          }
        }
      }
    }
    plot->setAstarPoint(points);
    plot->setCollision(collision);

    std::pair<int, int> start{(x_path[1]+4.5)*100, (y_path[1]+3)*100};
    std::pair<int, int> goal{(x_path[0]+4.5)*100, (y_path[0]+3)*100};
    int start_x = (int)(start.first/distance) * distance;
    int start_y = (int)(start.second/distance) * distance;
    int goal_x = (int)(goal.first/distance) * distance;
    int goal_y = (int)(goal.second/distance) * distance;
    std::vector<std::pair<int, int>> start_area {
      std::pair{start_x, start_y},
      std::pair{start_x+distance, start_y},
      std::pair{start_x, start_y+distance},
      std::pair{start_x+distance, start_y+distance},
    };
    std::vector<std::pair<int, int>> goal_area {
      std::pair{goal_x, goal_y},
      std::pair{goal_x+distance, goal_y},
      std::pair{goal_x, goal_y+distance},
      std::pair{goal_x+distance, goal_y+distance},
    };
    AStar::Vec2i start_point, goal_point;
    int min_value = INT_MAX;
    for (int i = 0; i < 4; i++) {
      int value = sqrt(pow(start_area[i].first-start.first, 2)+pow(start_area[i].second-start.second, 2))
        + sqrt(pow(start_area[i].first-goal.first, 2)+pow(start_area[i].second-goal.second, 2));
      if (min_value > value) {
        min_value = value;
        start_point = AStar::Vec2i{start_area[i].first/distance, start_area[i].second/distance};
      }
    }
    min_value = INT_MAX;
    for (int i = 0; i < 4; i++) {
      int value = sqrt(pow(goal_area[i].first-start.first, 2)+pow(goal_area[i].second-start.second, 2))
        + sqrt(pow(goal_area[i].first-goal.first, 2)+pow(goal_area[i].second-goal.second, 2));
      if (min_value > value) {
        min_value = value;
        goal_point = AStar::Vec2i{goal_area[i].first/distance, goal_area[i].second/distance};
      }
    }

    AStar::CoordinateList path = generator->findPath(start_point, goal_point, distance);
    std::vector<std::pair<int,int>> path_points;
    path_points.push_back(std::pair{x_path[0]*100, y_path[0]*100});
    for (int i = 0; i < path.size(); i++) {
      path_points.push_back(std::pair{
        (path[i].x*distance/100.0 - 4.5)*100,
        (path[i].y*distance/100.0 - 3)*100
      });
    }
    path_points.push_back(std::pair{x_path[1]*100, y_path[1]*100});
    plot->setPathPosition(path_points);
    std::vector<std::pair<int,int>> bezier_path;
    calc_path(path_points, bezier_path);
    plot->setBezierPath(bezier_path);
    std::reverse(bezier_path.begin(), bezier_path.end());
    // write_path_astar(path_points);

    bool isWalking = false;
    bool isControl = false;
    int key, index = 0;
    while(true) {
      walk->fallen();
      walk->setVel(0,0);

      plot->setRobotPosition(
        walk->gps->getValues()[0]*100,
        walk->gps->getValues()[1]*100);
      plot->setRobotDirection(walk->getDir());

      if (isControl) {
        while ((key = walk->keyboard->getKey()) >= 0) {
          switch (key) {
            case ' ':
              if (isWalking) {
                walk->run(false);
                isWalking = false;
              }
              else {
                walk->run(true);
                isWalking = true;
              }
            case webots::Keyboard::UP:
              walk->setVel(1,0);
              break;
            case webots::Keyboard::DOWN:
              walk->setVel(-1,0);
              break;
            case webots::Keyboard::RIGHT:
              walk->setVel(0,-0.5);
              break;
            case webots::Keyboard::LEFT:
              walk->setVel(0,0.5);
              break;
          }
        }
      } else {
        if (!isWalking) {
          walk->run(true);
          isWalking = true;
        }

        int dir = walk->getDir() * 180.0 / M_PI;
        int x = walk->gps->getValues()[0]*100;
        int y = walk->gps->getValues()[1]*100;
        int x_target = bezier_path[index].first;
        int y_target = bezier_path[index].second;
        double distance = sqrt(pow(x-x_target, 2) + pow(y-y_target, 2));
        int next_index = 0;
        while (distance < 40 && index + next_index < bezier_path.size()) {
          int next_x = bezier_path[index+next_index].first;
          int next_y = bezier_path[index+next_index].second;
          distance = sqrt(pow(x-next_x, 2) + pow(y-next_y, 2));
          next_index++;
        }
        index += next_index;

        plot->setTarget(x_target, y_target);

        if (index == bezier_path.size()) {
          walk->run(false);
          isWalking = false;
          isControl = true;
        }

        int target_dir = atan2(y-y_target, x_target-x) * 180.0 / M_PI;
        int delta_dir = target_dir-dir;

        double x_speed = smoothValue(abs(delta_dir), 0, 60, 1.0, 0.0);
        double a_speed = smoothValue(delta_dir, -60, 60, 1.0, -1.0);

        walk->setVel(x_speed, a_speed);
      }

      walk->robotStep();
      app->processEvents();
    }
}

