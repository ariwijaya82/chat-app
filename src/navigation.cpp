#include "controller.hpp"
#include "path_generator.hpp"
#include "monitor.hpp"
#include "utils.hpp"

#include <chrono>

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

    Constant c;
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();

    // Update Process
    bool isWalking = false;
    bool isControl = false;
    bool isTracking = true;
    size_t index = 0;
    
    size_t second_index = 0;
    double error_tracking = 0;
    int count_tracking = 0;

    while(true) {
      controller->setVel(0,0);

      // update position adn direction in radian
      Vec robot_position = controller->getPosition();
      monitor->setRobotPosition(robot_position);
      monitor->setRobotDirection(controller->getDirInRadian());

      if (isControl) {
        controller->manualController();
      } else if (isTracking) {
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

        double min_err = (robot_position - generator->bezier_path[second_index]).len();
        for (size_t i = second_index; i < index; i++) {
          double new_err = (robot_position-generator->bezier_path[i]).len();
          if (min_err > new_err) {
            second_index = i;
            min_err = new_err;
          }
        }

        error_tracking += min_err*min_err;
        count_tracking += 1;

        monitor->setTarget(target);
        if (index == generator->bezier_path.size()) {
          controller->run(false);
          isWalking = false;
          isControl = true;

          chrono::steady_clock::time_point end = chrono::steady_clock::now();
          cout << "time elapsed: " << chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "s" << endl;
          cout << "error: " << sqrt(error_tracking / count_tracking) << endl;
          isTracking = false;
        }

        double robot_direction = controller->getDirInDegree();
        int target_dir = atan2(robot_position.y - target.y, target.x - robot_position.x) * 180.0 / M_PI;
        int delta_dir = target_dir - robot_direction;

        double x_speed = smoothValue(abs(delta_dir), 0, c.range_x, 1.0, 0.0);
        double a_speed = smoothValue(delta_dir, -c.range_a, c.range_a, 1.0, -1.0);

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

