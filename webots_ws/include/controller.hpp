#ifndef __CONTROLLER_HPP__
#define __CONTROLLER_HPP__

#include <RobotisOp2GaitManager.hpp>
#include <RobotisOp2MotionManager.hpp>

#include <webots/Keyboard.hpp>
#include <webots/Motor.hpp>
#include <webots/PositionSensor.hpp>
#include <webots/Robot.hpp>
#include <webots/Gyro.hpp>
#include <webots/Accelerometer.hpp>
#include <webots/GPS.hpp>
#include <webots/Compass.hpp>

#include <vector>
#include <string>

#include "utils.hpp"

using namespace std;

class Controller {
    public:
        Controller(GlobalData*);
        ~Controller();
        
        void process();
        void run(bool);
        void setTarget(Vec);
        void setManual(bool);

        double getDirInRadian();
        double getDirInDegree();
        Vec getPosition();
        Vec getTarget();

        string getName() { return robot->getName(); }
        bool getIsFinished() { return isFinished; }

    private:
        GlobalData* global;

        webots::Robot *robot;
        webots::Keyboard *keyboard;
        webots::GPS *gps;
        webots::Compass *compass;
        managers::RobotisOp2MotionManager *motionManager;
        managers::RobotisOp2GaitManager *gaitManager;
        
        int timeStep;
        bool isWalking = false,
             isManual = false,
             isFinished = true;

        Vec target_point;

        void wait(int ms);
        double mappingValue(double, double, double, double, double);
        void checkIfFallen();
};

#endif