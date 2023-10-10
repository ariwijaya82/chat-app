#include "controller.hpp"
#include "path_generator.hpp"
#include "monitor.hpp"
#include "utils.hpp"

#include <chrono>

int main(int argc, char** argv) {
    // initialize controller
    Controller* controller = new Controller();
    GlobalData *global = GlobalData::getInstance();

    // initialize global->bezier_path generator
    PathGenerator* generator = new PathGenerator();
    generator->generatePath();
    generator->generateSmoothPath(100);

    // initialize monitor
    QApplication* app = new QApplication(argc, argv);
    Monitor* monitor = new Monitor();
    monitor->setDisableButton(true);
    monitor->setAStarPath(global->astar_path);
    monitor->setBezierPath(global->bezier_path);
    monitor->setNodeVisited(global->visited_node.size());
    monitor->setAStarLength(generator->getAstarLength());
    monitor->setBezierLength(generator->getBezierLength());
    monitor->show();

    // Update Process
    bool isWalking = false;
    bool isControl = false;
    bool isTracking = true;
    size_t index = 0;
    
    size_t second_index = 0;
    double error_tracking = 0;
    int count_tracking = 0;

    chrono::steady_clock::time_point begin = chrono::steady_clock::now();

    while(true) {
      controller->setVel(0,0);

      if (isControl) {
        controller->manualController();
      } else if (isTracking){
        if (!isWalking) {
          controller->run(true);
          isWalking = true;
        }

        // update position adn direction in radian
        Vec robot_position = controller->getPosition();
        monitor->setRobotPosition(robot_position);
        monitor->setRobotDirection(controller->getDirInRadian());
        monitor->addFollowingPath(robot_position);

        Vec target = global->bezier_path[index];
        double distance = (target - robot_position).len();
        size_t next_index = 0;
        while (distance < 20 && index + next_index < global->bezier_path.size()) {
          Vec next_target = global->bezier_path[index+next_index];
          distance = (next_target - robot_position).len();
          next_index++;
        }
        index += next_index;

        double min_err = (robot_position - global->bezier_path[second_index]).len();
        for (size_t i = second_index; i < index; i++) {
          double new_err = (robot_position-global->bezier_path[i]).len();
          if (min_err > new_err) {
            second_index = i;
            min_err = new_err;
          }
        }

        error_tracking += min_err*min_err;
        count_tracking += 1;

        monitor->setTarget(target);
        if (index == global->bezier_path.size()) {
          controller->run(false);
          isControl = true;
          isTracking = false;

          chrono::steady_clock::time_point end = chrono::steady_clock::now();
          double delta_time = chrono::duration_cast<std::chrono::seconds>(end - begin).count();
          monitor->setTimeFollow(delta_time);
          monitor->setFollowingError(sqrt(error_tracking / count_tracking));
          monitor->setDisableButton(false);
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

    delete controller;
    delete generator;
    delete app;
    delete monitor;
    return 0;
}

