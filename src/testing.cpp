#include "path_generator.hpp"
#include "monitor.hpp"
#include "utils.hpp"

// #include <iostream>
// #include <fstream>
// #include <nlohmann/json.hpp>

// using namespace std;
// using nlohmann::json;

int main(int argc, char** argv) {
    QApplication* app = new QApplication(argc, argv);
    Monitor* monitor = new Monitor(16, "other");
    monitor->show();

    // Calculate Path
    // PathGenerator* generator = new PathGenerator();

    // Draw Path
    // monitor->setAStarPath(generator->astar_path);
    // monitor->setBezierPath(generator->bezier_path);

    return app->exec();

    // ifstream file("config/global.json");
    // json data = json::parse(file);
    // cout << data["screen_height"] << endl;
}