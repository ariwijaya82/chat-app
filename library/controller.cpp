#include "controller.hpp"

static const char *motorNames[20] = {
  "ShoulderR" /*ID1 */, "ShoulderL" /*ID2 */, "ArmUpperR" /*ID3 */, "ArmUpperL" /*ID4 */, "ArmLowerR" /*ID5 */,
  "ArmLowerL" /*ID6 */, "PelvYR" /*ID7 */,    "PelvYL" /*ID8 */,    "PelvR" /*ID9 */,     "PelvL" /*ID10*/,
  "LegUpperR" /*ID11*/, "LegUpperL" /*ID12*/, "LegLowerR" /*ID13*/, "LegLowerL" /*ID14*/, "AnkleR" /*ID15*/,
  "AnkleL" /*ID16*/,    "FootR" /*ID17*/,     "FootL" /*ID18*/,     "Neck" /*ID19*/,      "Head" /*ID20*/
};

Controller::Controller(string type) {
    robot = new webots::Robot();
    timeStep = robot->getBasicTimeStep();

    robot_type = type;
    if (type == "main") {
        compass = robot->getCompass("compass");
        gps = robot->getGPS("gps");
        keyboard = robot->getKeyboard();
        
        compass->enable(timeStep);
        gps->enable(timeStep);
        keyboard->enable(timeStep);
    }

    gyro = robot->getGyro("Gyro");
    gyro->enable(timeStep);
    for (int i = 0; i < 20; i++) {
        std::string sensorName = motorNames[i];
        sensorName.push_back('S');
        positionSensors[i] = robot->getPositionSensor(sensorName);
        positionSensors[i]->enable(timeStep);
    }

    motionManager = new managers::RobotisOp2MotionManager(robot);
    gaitManager = new managers::RobotisOp2GaitManager(robot, "../config/walking.ini");

    robot->step(timeStep);
    motionManager->playPage(9);
    wait(200);
}

Controller::~Controller() {
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

void Controller::run(bool start) {
    if (start) gaitManager->start();
    else gaitManager->stop();
}

void Controller::setVel(double x, double a) {
    gaitManager->setXAmplitude(x);
    gaitManager->setAAmplitude(a);
}

void Controller::robotStep() {
    robot->step(timeStep);
    gaitManager->step(timeStep);
}


void Controller::manualController() {
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

double Controller::getDirInRadian() {
    double dir = atan2(compass->getValues()[1], compass->getValues()[0]);
    dir -= M_PI/2;
    if (dir < -M_PI) dir += M_PI*2;
    return dir;
}

double Controller::getDirInDegree() {    
    return getDirInRadian() * 180 / M_PI;
}

Vec Controller::getPosition() {
    return Vec((gps->getValues()[0] + 4.5) * 100, (gps->getValues()[1] + 3) * 100);
}

int Controller::getTimeStep() {
    return timeStep;
}

void Controller::wait(int ms) {
    double start = robot->getTime();
    double s = (double)ms / 1000.0;
    while(start + s < robot->getTime())
        robotStep();
}