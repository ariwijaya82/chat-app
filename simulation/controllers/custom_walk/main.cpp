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

#include <cmath>
#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

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
        Compass *mCompass;

        RobotisOp2MotionManager *mMotionManager;
        RobotisOp2GaitManager *mGaitManager;
};

// initial
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
    mCompass = getCompass("compass");
    mCompass->enable(mTime);
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

// calculation
double get_direction_in_degrees(double x, double y) {
  double rad = atan2(x, y);
  rad *= 180.0/M_PI;
  return rad;
}

void readPath(vector<double> &x_path, vector<double> &y_path) {
  string filename = "path.csv";
  fstream file(filename, ios::in);
  string line, value;
  if (file.is_open()) {
    while (getline(file, line)) {
      std::stringstream str(line);
      getline(str, value, ',');
      x_path.push_back(stod(value));
      getline(str, value, ',');
      y_path.push_back(stod(value));
    }
  }
}

int smoothValue(double value, double min_input, double max_input, double min_output, double max_output) {
  double clamp_value = value;
  if (value < min_input) clamp_value = min_input;
  if (value > max_input) clamp_value = max_output;
  cout << "clamp: " << clamp_value*(max_output-min_output)/(max_input-min_input) << endl
       << "min output" << min_output << endl; 
  return min_output + (clamp_value-min_input)*(max_output-min_output)/(max_input-min_input);
}

void Walk::run() {
  cout << "-------Walk ROBOTIS OP2-------" << endl;
  cout << "Control with keyboard" << endl;

  // First step to update sensors values
  myStep();

  // play the hello motion
  mMotionManager->playPage(9);  // init position
  wait(200);

  // main loop
  bool isWalking = false;
  const double radius_range = 0.1;
  double x_diff, y_diff, radius= 0.0;
  int index = 0;
  vector<double> x_path, y_path;

  while (true) {
    checkIfFallen();

    mGaitManager->setXAmplitude(0.0);
    mGaitManager->setAAmplitude(0.0);

    // get keyboard key
    int key = 0;
    while ((key = mKeyboard->getKey()) >= 0) {
      switch (key) {
        case ' ':  // Space bar
          readPath(x_path, y_path);
          cout << x_path.size() << endl;
          if (isWalking) {
            mGaitManager->stop();
            isWalking = false;
            cout << "stop walking" << endl;
            wait(200);
          } else {
            mGaitManager->start();
            isWalking = true;
            cout << "start walking" << endl;
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

    if (isWalking) {
      while (radius < radius_range) {
        index++;
        x_diff = x_path[index] - mGPS->getValues()[0];
        y_diff = y_path[index] - mGPS->getValues()[1];
        radius = sqrt(x_diff*x_diff + y_diff*y_diff);
      }
      x_diff = x_path[index] - mGPS->getValues()[0];
      y_diff = y_path[index] - mGPS->getValues()[1];
      radius = sqrt(x_diff*x_diff + y_diff*y_diff);
      
      cout << "index: " << index << endl;
      // cout << "x_tar: " << x_path[index]
      //      << ", y_tar: " << y_path[index]
      //      << ", x_curr: " << mGPS->getValues()[0]
      //      << ", y_curr: " << mGPS->getValues()[1]
      //      << ", x_diff: " << x_diff
      //      << ", y_diff: " << y_diff << endl;

      double target_dir = get_direction_in_degrees(x_diff, y_diff);
      double curr_dir = get_direction_in_degrees(mCompass->getValues()[1], mCompass->getValues()[0])-90.0;
      double diff_dir = target_dir - curr_dir;
      cout << "target dir: " << target_dir << endl
           << "current dir: " << curr_dir << endl
           << "diff dir " << diff_dir << endl;
      // if (diff_dir > 180.0) diff_dir -= 360.0;
      // if (diff_dir < -180.0) diff_dir += 360.0;

      double x_vel = smoothValue(abs(diff_dir), 0, 45, 30, 0);
      double a_vel = smoothValue(diff_dir, -20, 20, -20, 20);
      cout << "x: " << x_vel << endl;
      cout << "a: " << a_vel << endl;
      mGaitManager->setXAmplitude(x_vel);
      mGaitManager->setAAmplitude(a_vel);
      // return;
    }

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