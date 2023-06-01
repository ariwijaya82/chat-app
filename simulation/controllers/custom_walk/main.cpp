#include <RobotisOp2GaitManager.hpp>
#include <RobotisOp2MotionManager.hpp>

#include <webots/Accelerometer.hpp>
#include <webots/Gyro.hpp>
#include <webots/Keyboard.hpp>
#include <webots/Motor.hpp>
#include <webots/PositionSensor.hpp>
#include <webots/Robot.hpp>
#include <webots/GPS.hpp>

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace webots;
using namespace managers;
using namespace std;

static const char *motorNames[20] = {
  "ShoulderR" /*ID1 */, "ShoulderL" /*ID2 */, "ArmUpperR" /*ID3 */, "ArmUpperL" /*ID4 */, "ArmLowerR" /*ID5 */,
  "ArmLowerL" /*ID6 */, "PelvYR" /*ID7 */,    "PelvYL" /*ID8 */,    "PelvR" /*ID9 */,     "PelvL" /*ID10*/,
  "LegUpperR" /*ID11*/, "LegUpperL" /*ID12*/, "LegLowerR" /*ID13*/, "LegLowerL" /*ID14*/, "AnkleR" /*ID15*/,
  "AnkleL" /*ID16*/,    "FootR" /*ID17*/,     "FootL" /*ID18*/,     "Neck" /*ID19*/,      "Head" /*ID20*/
};

class Walk : public Robot {
    public:
        Walk();
        void run();
        void checkIfFallen();

    private:
        int mTime;

        void myStep();
        void wait(int ms);

        PositionSensor *mPositionSensors[20];
        Accelerometer *mAccelerometer;
        Gyro *mGyro;
        Keyboard *mKeyboard;
        GPS *mGPS;

        RobotisOp2MotionManager *mMotionManager;
        RobotisOp2GaitManager *mGaitManager;
};

Walk::Walk() : Robot() {
    mTime = getBasicTimeStep();
    mAccelerometer = getAccelerometer("Accelerometer");
    mAccelerometer->enable(mTime);
    mGyro = getGyro("Gyro");
    mGyro->enable(mTime);
    mKeyboard = getKeyboard();
    mKeyboard->enable(mTime);
    mGPS = getGPS("gps");
    mGPS->enable(mTime);
    for (int i = 0; i < 20; i++) {
        string sensorName = motorNames[i];
        sensorName.push_back('S');
        mPositionSensors[i] = getPositionSensor(sensorName);
        mPositionSensors[i]->enable(mTime);
    }

    mMotionManager = new RobotisOp2MotionManager(this);
    mGaitManager = new RobotisOp2GaitManager(this, "config.ini");
}

void Walk::myStep() {
    int ret = step(mTime);
    if (ret == -1) exit(EXIT_SUCCESS);
}

void Walk::wait(int ms) {
    double startTime = getTime();
    double s = (double)ms / 1000.0;
    while(s + startTime >= getTime())
        myStep();
}

void Walk::run() {
  cout << "-------Walk ROBOTIS OP2-------" << endl;
  cout << "This example illustrates Gait Manager" << endl;
  cout << "Press the space bar to start/stop walking" << endl;
  cout << "Use the arrow keys to move the robot while walking" << endl;

  // First step to update sensors values
  myStep();

  // play the hello motion
  mMotionManager->playPage(9);  // init position
  wait(200);

  // main loop
  bool isWalking = false;

  while (true) {
    checkIfFallen();

    mGaitManager->setXAmplitude(0.0);
    mGaitManager->setAAmplitude(0.0);

    // get keyboard key
    int key = 0;
    while ((key = mKeyboard->getKey()) >= 0) {
      switch (key) {
        case ' ':  // Space bar
          if (isWalking) {
            mGaitManager->stop();
            isWalking = false;
            wait(200);
          } else {
            mGaitManager->start();
            isWalking = true;
            wait(200);
          }
          break;
        case Keyboard::UP:
          mGaitManager->setXAmplitude(10.0);
          break;
        case Keyboard::DOWN:
          mGaitManager->setXAmplitude(0.0);
          break;
        case Keyboard::RIGHT:
          mGaitManager->setAAmplitude(-0.5);
          break;
        case Keyboard::LEFT:
          mGaitManager->setAAmplitude(0.5);
          break;
      }
    }

    cout << "x: " << mGPS->getValues()[0] << ", "
         << "y: " << mGPS->getValues()[1] << ", "
         << "z: " << mGPS->getValues()[2] << endl;

    mGaitManager->step(mTime);
    myStep();
  }
}

void Walk::checkIfFallen() {
  static int fup = 0;
  static int fdown = 0;
  static const double acc_tolerance = 80.0;
  static const double acc_step = 100;

  // count how many steps the accelerometer
  // says that the robot is down
  const double *acc = mAccelerometer->getValues();
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
    mMotionManager->playPage(10);  // f_up
    mMotionManager->playPage(9);   // init position
    fup = 0;
  }
  // the back face is down
  else if (fdown > acc_step) {
    mMotionManager->playPage(11);  // b_up
    mMotionManager->playPage(9);   // init position
    fdown = 0;
  }
}

int main() {
    Walk *walk = new Walk();
    walk->run();
    delete walk;
    return EXIT_FAILURE;
}