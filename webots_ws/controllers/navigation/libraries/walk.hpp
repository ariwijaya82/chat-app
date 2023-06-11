#ifndef __WALK_HPP__
#define __WALK_HPP__

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

#include <string>

class Walk {
    public:
    Walk();
    void run(bool start);
    void setVel(double x, double a);
    void wait(int ms);

    void robotStep();
    void fallen();

    double getDir();

    webots::Robot *robot;
    webots::Accelerometer *accelerometer;
    webots::Gyro *gyro;
    webots::Keyboard *keyboard;
    webots::GPS *gps;
    webots::Compass *compass;
    webots::PositionSensor *positionSensors[20];


    private:
    int timeStep;
    managers::RobotisOp2MotionManager *motionManager;
    managers::RobotisOp2GaitManager *gaitManager;
};

#endif