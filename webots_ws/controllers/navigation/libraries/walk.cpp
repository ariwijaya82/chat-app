#include "walk.hpp"

static const char *motorNames[20] = {
  "ShoulderR" /*ID1 */, "ShoulderL" /*ID2 */, "ArmUpperR" /*ID3 */, "ArmUpperL" /*ID4 */, "ArmLowerR" /*ID5 */,
  "ArmLowerL" /*ID6 */, "PelvYR" /*ID7 */,    "PelvYL" /*ID8 */,    "PelvR" /*ID9 */,     "PelvL" /*ID10*/,
  "LegUpperR" /*ID11*/, "LegUpperL" /*ID12*/, "LegLowerR" /*ID13*/, "LegLowerL" /*ID14*/, "AnkleR" /*ID15*/,
  "AnkleL" /*ID16*/,    "FootR" /*ID17*/,     "FootL" /*ID18*/,     "Neck" /*ID19*/,      "Head" /*ID20*/
};

Walk::Walk() {
    robot = new webots::Robot();
    timeStep = robot->getBasicTimeStep();

    accelerometer = robot->getAccelerometer("Accelerometer");
    gyro = robot->getGyro("Gyro");
    gps = robot->getGPS("gps");
    compass = robot->getCompass("compass");
    keyboard = robot->getKeyboard();
    for (int i = 0; i < 20; i++) {
        std::string sensorName = motorNames[i];
        sensorName.push_back('S');
        positionSensors[i] = robot->getPositionSensor(sensorName);
        positionSensors[i]->enable(timeStep);
    }
    accelerometer->enable(timeStep);
    gyro->enable(timeStep);
    gps->enable(timeStep);
    compass->enable(timeStep);
    keyboard->enable(timeStep);

    motionManager = new managers::RobotisOp2MotionManager(robot);
    gaitManager = new managers::RobotisOp2GaitManager(robot, "../data/config.ini");

    robot->step(timeStep);
    motionManager->playPage(9);
    wait(200);
}

void Walk::run(bool start) {
    if (start) gaitManager->start();
    else gaitManager->stop();
}

void Walk::setVel(double x, double a) {
    gaitManager->setXAmplitude(x);
    gaitManager->setAAmplitude(a);
}

void Walk::robotStep() {
    robot->step(timeStep);
    gaitManager->step(timeStep);
}

double Walk::getDir() {    
    double dir = atan2(compass->getValues()[1], compass->getValues()[0]);
    dir -= M_PI/2;
    if (dir < -M_PI) dir += M_PI*2;
    return dir;
}

void Walk::fallen() {
    static int fup = 0;
    static int fdown = 0;
    static const double acc_tolerance = 80.0;
    static const double acc_step = 100;

    const double *acc = accelerometer->getValues();
    if (acc[1] < 512.0 - acc_tolerance)
        fup++;
    else
        fup = 0;

    if (acc[1] > 512.0 + acc_tolerance)
        fdown++;
    else
        fdown = 0;

    if (fup > acc_step) {
        motionManager->playPage(10);  // f_up
        motionManager->playPage(9);   // init position
        fup = 0;
    } else if (fdown > acc_step) {
        motionManager->playPage(11);  // b_up
        motionManager->playPage(9);   // init position
        fdown = 0;
    }
}

void Walk::wait(int ms) {
    double start = robot->getTime();
    double s = (double)ms / 1000.0;
    while(start + s < robot->getTime())
        robot->step(timeStep);
}