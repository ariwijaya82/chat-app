#include "controller.hpp"

const char *positionNames[20] = {
  "ShoulderRS" /*ID1 */, "ShoulderLS" /*ID2 */, "ArmUpperRS" /*ID3 */, "ArmUpperLS" /*ID4 */, "ArmLowerRS" /*ID5 */,
  "ArmLowerLS" /*ID6 */, "PelvYRS" /*ID7 */,    "PelvYLS" /*ID8 */,    "PelvRS" /*ID9 */,     "PelvLS" /*ID10*/,
  "LegUpperRS" /*ID11*/, "LegUpperLS" /*ID12*/, "LegLowerRS" /*ID13*/, "LegLowerLS" /*ID14*/, "AnkleRS" /*ID15*/,
  "AnkleLS" /*ID16*/,    "FootRS" /*ID17*/,     "FootLS" /*ID18*/,     "NeckS" /*ID19*/,      "HeadS" /*ID20*/
};

Controller::Controller(GlobalData* global_) {
    robot = new webots::Robot();
    timeStep = robot->getBasicTimeStep();

    compass = robot->getCompass("compass");
    compass->enable(timeStep);
    gps = robot->getGPS("gps");
    gps->enable(timeStep);
    robot->getGyro("Gyro")->enable(timeStep);
    robot->getAccelerometer("Accelerometer")->enable(timeStep);
    for (int i = 0; i < 20; i++) {
      robot->getPositionSensor(positionNames[i])
           ->enable(timeStep);
    }
    keyboard = robot->getKeyboard();
    keyboard->enable(timeStep);

    motionManager = new managers::RobotisOp2MotionManager(robot);
    gaitManager = new managers::RobotisOp2GaitManager(robot, "../../config/walking.ini");

    global = global_;

    robot->step(timeStep);
    motionManager->playPage(9);
    wait(200);
}

Controller::~Controller() {
    delete robot;
    delete motionManager;
    delete gaitManager;
}

void Controller::process() {
  checkIfFallen();
  
  gaitManager->setXAmplitude(0.0);
  gaitManager->setAAmplitude(0.0);

  if (isManual) {
    int key = 0;
    while ((key = keyboard->getKey()) >= 0) {
      switch (key) {
        case ' ':
          if (isWalking) {
            run(false);
            wait(200);
          } else {
            run(true);
            wait(200);
          }
          break;
        
        case webots::Keyboard::UP:
          gaitManager->setXAmplitude(1.0);
          break;
        
        case webots::Keyboard::DOWN:
          gaitManager->setXAmplitude(-1.0);
          break;

        case webots::Keyboard::LEFT:
          gaitManager->setAAmplitude(0.5);
          break;

        case webots::Keyboard::RIGHT:
          gaitManager->setAAmplitude(-0.5);
          break;

        default:
          break;
      }
    }
  }
  
  if (!isFinished) {
    Vec delta = target_point - getPosition();
    if (delta.len() < global->robot_radius/2) {
      isFinished = true;
    }
    double target_dir = atan2(-delta.y, delta.x) * 180.0 / M_PI;
    double delta_dir = target_dir - getDirInDegree();

    gaitManager->setXAmplitude(mappingValue(abs(delta_dir), 0, 60, 1.0, 0.0));
    gaitManager->setAAmplitude(mappingValue(delta_dir, -60, 60, 1.0, -1.0));
  }

  gaitManager->step(timeStep);
  robot->step(timeStep);
}

void Controller::run(bool start) {
  if (start) {
    gaitManager->start();
    isWalking = true;
  } else {
    gaitManager->stop();
    isWalking = false;
  }
}

void Controller::setTarget(Vec target) {
  isFinished = false;
  target_point = target;
}

void Controller::setManual(bool value) {
  isManual = value;
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

Vec Controller::getTarget() {
  if (!isFinished) {
    return target_point;
  } else return getPosition();
}

void Controller::wait(int ms) {
    double start = robot->getTime();
    double s = (double)ms / 1000.0;
    while(start + s > robot->getTime()) {
        robot->step(timeStep);
    }
}

double Controller::mappingValue(double value, double min_input, double max_input, double min_output, double max_output) {
  double clamp_value = value;
  if (value < min_input) clamp_value = min_input;
  if (value > max_input) clamp_value = max_input;
  return min_output + (clamp_value-min_input)*(max_output-min_output)/(max_input-min_input);
}

void Controller::checkIfFallen() {
  static int fup = 0;
  static int fdown = 0;
  static const double acc_tolerance = 80.0;
  static const double acc_step = 100;

  // count how many steps the accelerometer
  // says that the robot is down
  const double *acc = robot->getAccelerometer("Accelerometer")->getValues();
  if (acc[1] < 512.0 - acc_tolerance)
    fup++;
  else
    fup = 0;

  if (acc[1] > 512.0 + acc_tolerance)
    fdown++;
  else
    fdown = 0;

  // the robot face is down
  if (fup > acc_step) {
    motionManager->playPage(10);  // f_up
    motionManager->playPage(9);   // init position
    fup = 0;
  }
  // the back face is down
  else if (fdown > acc_step) {
    motionManager->playPage(11);  // b_up
    motionManager->playPage(9);   // init position
    fdown = 0;
  }
}