#include "robot_controller.hpp"
#include "path_planning.hpp"
#include "monitoring.hpp"

double smoothValue(double value, double min_input, double max_input, double min_output, double max_output) {
  double clamp_value = value;
  if (value < min_input) clamp_value = min_input;
  if (value > max_input) clamp_value = max_input;
  return min_output + (clamp_value-min_input)*(max_output-min_output)/(max_input-min_input);
}

int main(int argc, char** argv) {
    // initialize QT
    QApplication* app = new QApplication(argc, argv);
    Monitoring* monitor = new Monitoring();
    monitor->show();

    // initialize robot controller
    RobotController* controller = new RobotController();

    // Calculate Path
    PathPlanning* generator = new PathPlanning();

    // Draw Path
    vector<pair<int, int>> bezier_path = generator->getBezierPath();
    vector<pair<int, int>> path = generator->getPath();
    monitor->setTarget(generator->getStart());
    monitor->setRobotPosition(generator->getStart());
    monitor->setBallPosition(generator->getGoal());
    monitor->setEnemyPosition(generator->getEnemy());
    monitor->setCollision(generator->getCollision());
    monitor->setPath(path);
    monitor->setBezierPath(bezier_path);

    // Update Process
    bool isWalking = false;
    bool isControl = false;
    int key, index = 0;
    while(true) {
      controller->fallen();
      controller->setVel(0,0);

      // update position adn direction in radian
      auto gps_value = controller->getPosition();
      pair<int, int> robot_position{(gps_value.first + 4.5) * 100, (gps_value.second + 3) * 100};
      monitor->setRobotPosition(robot_position);
      monitor->setRobotDirection(controller->getDirInRadian());

      if (isControl) {
        controller->manualController();
      } else {
        if (!isWalking) {
          controller->run(true);
          isWalking = true;
        }

        pair<int, int> target = bezier_path[index];
        double distance = sqrt(pow(target.first - robot_position.first, 2) + pow(target.second - robot_position.second, 2));
        int next_index = 0;
        while (distance < 40 && index + next_index < bezier_path.size()) {
          pair<int, int> next_target = bezier_path[index+next_index];
          distance = sqrt(pow(next_target.first - robot_position.first, 2) + pow(next_target.second - robot_position.second, 2));
          next_index++;
        }
        index += next_index;

        monitor->setTarget(target);
        if (index == bezier_path.size()) {
          controller->run(false);
          isWalking = false;
          isControl = true;
        }

        double robot_direction = controller->getDirInDegree();
        int target_dir = atan2(robot_position.second - target.second, target.first - robot_position.first) * 180.0 / M_PI;
        int delta_dir = target_dir - robot_direction;

        double x_speed = smoothValue(abs(delta_dir), 0, 60, 1.0, 0.0);
        double a_speed = smoothValue(delta_dir, -60, 60, 1.0, -1.0);

        controller->setVel(x_speed, a_speed);
      }

      controller->robotStep();
      app->processEvents();
    }
}

