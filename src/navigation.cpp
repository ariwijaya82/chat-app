#include "controller.hpp"
#include "path_generator.hpp"
#include "monitor.hpp"
#include "utils.hpp"

double smoothValue(double value, double min_input, double max_input, double min_output, double max_output) {
  double clamp_value = value;
  if (value < min_input) clamp_value = min_input;
  if (value > max_input) clamp_value = max_input;
  return min_output + (clamp_value-min_input)*(max_output-min_output)/(max_input-min_input);
}

int main(int argc, char** argv) {
    // initialize controller
    Controller* controller = new Controller("main");

    // initialize monitor
    QApplication* app = new QApplication(argc, argv);
    Monitor* monitor = new Monitor(controller->robot->getBasicTimeStep(), "main");
    monitor->show();

    // initialize path generator
    PathGenerator* generator = new PathGenerator();
    monitor->setAStarPath(generator->astar_path);
    monitor->setBezierPath(generator->bezier_path);

    // Update Process
    bool isWalking = false;
    bool isControl = false;
    size_t index = 0;
    while(true) {
      controller->setVel(0,0);

      // update position adn direction in radian
      Vec robot_position = controller->getPosition();
      monitor->setRobotPosition(robot_position);
      monitor->setRobotDirection(controller->getDirInRadian());

      if (isControl) {
        controller->manualController();
      } else {
        if (!isWalking) {
          controller->run(true);
          isWalking = true;
        }

        Vec target = generator->bezier_path[index];
        double distance = (target - robot_position).len();
        size_t next_index = 0;
        while (distance < 40 && index + next_index < generator->bezier_path.size()) {
          Vec next_target = generator->bezier_path[index+next_index];
          distance = (next_target - robot_position).len();
          next_index++;
        }
        index += next_index;

        monitor->setTarget(target);
        if (index == generator->bezier_path.size()) {
          controller->run(false);
          isWalking = false;
          isControl = true;
        }

        double robot_direction = controller->getDirInDegree();
        int target_dir = atan2(robot_position.y - target.y, target.x - robot_position.x) * 180.0 / M_PI;
        int delta_dir = target_dir - robot_direction;

        double x_speed = smoothValue(abs(delta_dir), 0, 60, 1.0, 0.0);
        double a_speed = smoothValue(delta_dir, -60, 60, 1.0, -1.0);

        controller->setVel(x_speed, a_speed);
      }

      controller->robotStep();
      app->processEvents();
    }

    delete app;
    delete monitor;
    delete controller;
    delete generator;
    return 0;
}

