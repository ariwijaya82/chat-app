#include "robot_controller.hpp"

static const char *motorNames[20] = {
  "ShoulderR" /*ID1 */, "ShoulderL" /*ID2 */, "ArmUpperR" /*ID3 */, "ArmUpperL" /*ID4 */, "ArmLowerR" /*ID5 */,
  "ArmLowerL" /*ID6 */, "PelvYR" /*ID7 */,    "PelvYL" /*ID8 */,    "PelvR" /*ID9 */,     "PelvL" /*ID10*/,
  "LegUpperR" /*ID11*/, "LegUpperL" /*ID12*/, "LegLowerR" /*ID13*/, "LegLowerL" /*ID14*/, "AnkleR" /*ID15*/,
  "AnkleL" /*ID16*/,    "FootR" /*ID17*/,     "FootL" /*ID18*/,     "Neck" /*ID19*/,      "Head" /*ID20*/
};

RobotController::RobotController(string type) {
    robot = new webots::Robot();
    timeStep = robot->getBasicTimeStep();

    robot_type = type;
    if (type == "main") {
        compass = robot->getCompass("compass");
        compass->enable(timeStep);
    }

    gps = robot->getGPS("gps");
    gyro = robot->getGyro("Gyro");
    keyboard = robot->getKeyboard();
    for (int i = 0; i < 20; i++) {
        std::string sensorName = motorNames[i];
        sensorName.push_back('S');
        positionSensors[i] = robot->getPositionSensor(sensorName);
        positionSensors[i]->enable(timeStep);
    }
    gps->enable(timeStep);
    gyro->enable(timeStep);
    keyboard->enable(timeStep);

    motionManager = new managers::RobotisOp2MotionManager(robot);
    gaitManager = new managers::RobotisOp2GaitManager(robot, "../config/walking.ini");

    robot->step(timeStep);
    motionManager->playPage(9);
    wait(200);
}

RobotController::~RobotController() {
    delete robot;
    delete keyboard;
    delete gps;
    delete compass;
    delete gyro;
    delete motionManager;
    delete gaitManager;
    for (int i = 0; i < 20; i++) {
        delete positionSensors[i];
    }
}

void RobotController::run(bool start) {
    if (start) gaitManager->start();
    else gaitManager->stop();
}

void RobotController::setVel(double x, double a) {
    gaitManager->setXAmplitude(x);
    gaitManager->setAAmplitude(a);
}

void RobotController::robotStep() {
    robot->step(timeStep);
    gaitManager->step(timeStep);
}


void RobotController::manualController() {
    int key;
    while ((key = keyboard->getKey()) >= 0) {
        switch (key) {
            case webots::Keyboard::PAGEUP:
                run(true);
                break;
            case webots::Keyboard::PAGEDOWN:
                run(false);
                break;
            case webots::Keyboard::UP:
                setVel(1,0);
                break;
            case webots::Keyboard::DOWN:
                setVel(-1,0);
                break;
            case webots::Keyboard::RIGHT:
                setVel(0,-0.5);
                break;
            case webots::Keyboard::LEFT:
                setVel(0,0.5);
                break;
        }
    }
}

double RobotController::getDirInRadian() {   
    if (robot_type == "main") {
        double dir = atan2(compass->getValues()[1], compass->getValues()[0]);
        dir -= M_PI/2;
        if (dir < -M_PI) dir += M_PI*2;
        return dir;
    }
    return 0.0;
}

double RobotController::getDirInDegree() {    
    return getDirInRadian() * 180 / M_PI;
}

pair<double, double> RobotController::getPosition() {
    return pair<double, double>{gps->getValues()[0], gps->getValues()[1]};
}

void RobotController::wait(int ms) {
    double start = robot->getTime();
    double s = (double)ms / 1000.0;
    while(start + s < robot->getTime())
        robot->step(timeStep);
}