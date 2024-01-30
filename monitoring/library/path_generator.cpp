#include "path_generator.hpp"

PathGenerator::PathGenerator() {
    param par = loadParameter();
    this->node_distance = par.node_distance;
    this->heuristic_type = par.heuristic_type;
    this->bezier_curvature = par.bezier_curvature;
    this->robot_radius = par.robot_radius;
    this->screen_width = par.screen_width;
    this->screen_height = par.screen_height;

    position pos = loadPosition(par.path_number);
    this->ball = pos.ball;
    this->robot = pos.robot;
    this->enemies = pos.enemies;

    for (int i = 0; i < (int)enemies.size(); i++) {
        this->obstacles.push_back(vector<Point>());
        updateObstacles(i);
    }
}

void PathGenerator::setRobotPos(){}
void PathGenerator::setBallPos(){}
void PathGenerator::setEnemyPos(int){}

void PathGenerator::generatePath() {

}

void PathGenerator::updateObstacles(int){}