#ifndef __ROBOT_CONTROLLER_HPP__
#define __ROBOT_CONTROLLER_HPP__

#include <RobotisOp2GaitManager.hpp>
#include <RobotisOp2MotionManager.hpp>

#include <webots/Accelerometer.hpp>
#include <webots/Gyro.hpp>
#include <webots/Keyboard.hpp>
#include <webots/Motor.hpp>
#include <webots/PositionSensor.hpp>
#include <webots/Robot.hpp>
#include <webots/GPS.hpp>
#include <webots/Compass.hpp>

#include <vector>

using namespace std;

class RobotController {
    public:
    RobotController();
    void run(bool start);
    void setVel(double x, double a);

    void robotStep();
    void fallen();
    void manualController();

    double getDirInRadian();
    double getDirInDegree();
    pair<double, double> getPosition();

    private:
        int timeStep;
        
        webots::Robot *robot;
        webots::Accelerometer *accelerometer;
        webots::Gyro *gyro;
        webots::Keyboard *keyboard;
        webots::GPS *gps;
        webots::Compass *compass;
        webots::PositionSensor *positionSensors[20];
        managers::RobotisOp2MotionManager *motionManager;
        managers::RobotisOp2GaitManager *gaitManager;
        
        void wait(int ms);
};

#endif