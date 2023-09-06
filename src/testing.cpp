#include "path_planning.hpp"
#include "monitoring.hpp"

// #include <iostream>
// #include <fstream>
// #include <nlohmann/json.hpp>

// using namespace std;
// using nlohmann::json;

int main(int argc, char** argv) {
    QApplication* app = new QApplication(argc, argv);
    Monitoring* monitor = new Monitoring();
    monitor->show();

    // Calculate Path
    PathPlanning* generator = new PathPlanning();

    // Draw Path
    vector<pair<double, double>> bezier_path = generator->getBezierPath();
    vector<pair<double, double>> path = generator->getPath();
    monitor->setTarget(generator->getStart());
    monitor->setRobotPosition(generator->getStart());
    monitor->setBallPosition(generator->getGoal());
    monitor->setEnemyPosition(generator->getEnemy());
    monitor->setCollision(generator->getCollision());
    monitor->setPath(path);
    monitor->setBezierPath(bezier_path);
    monitor->setRobotDirection(0);

    return app->exec();

    // ifstream file("config/global.json");
    // json data = json::parse(file);
    // cout << data["screen_height"] << endl;
}