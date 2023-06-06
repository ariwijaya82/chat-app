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

#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtCore/QString>

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

void wait(int ms);
void checkIfFallen();
void readPath(vector<double> &x_path, vector<double> &y_path);
double getDirection(double x, double y);
int smoothValue(double value, double min_input, double max_input, double min_output, double max_output);

QApplication *app;
QLabel *sensorValueLabel;

webots::Robot *robot;
int timeStep;
Accelerometer *accelerometer;
Gyro *gyro;
GPS *gps;
Compass *compass;
Keyboard *keyboard;
PositionSensor *positionSensors[20];

RobotisOp2MotionManager *motionManager;
RobotisOp2GaitManager *gaitManager;

int main(int argc, char** argv) {
    app = new QApplication(argc, argv);
    sensorValueLabel = new QLabel();
    sensorValueLabel->show();
    QString sensorValueText = "Sensor Value: 123";
    sensorValueLabel->setText(sensorValueText);

    robot = new webots::Robot();

    timeStep = robot->getBasicTimeStep();
    accelerometer = robot->getAccelerometer("Accelerometer");
    gyro = robot->getGyro("Gyro");
    gps = robot->getGPS("gps");
    compass = robot->getCompass("compass");
    keyboard = robot->getKeyboard();
    for (int i = 0; i < 20; i++) {
        string sensorName = motorNames[i];
        sensorName.push_back('S');
        positionSensors[i] = robot->getPositionSensor(sensorName);
        positionSensors[i]->enable(timeStep);
    }
    accelerometer->enable(timeStep);
    gyro->enable(timeStep);
    gps->enable(timeStep);
    compass->enable(timeStep);
    keyboard->enable(timeStep);

    motionManager = new RobotisOp2MotionManager(robot);
    gaitManager = new RobotisOp2GaitManager(robot, "config.ini");

    cout << "Custom Walking" << endl;
    cout << "==============" << endl;

    robot->step(timeStep);
    motionManager->playPage(9);
    wait(200);
    bool isWalking = false; 
    int key = 0;

    while(true) {
      checkIfFallen();

      gaitManager->setXAmplitude(10.0);

      while ((key = keyboard->getKey()) >= 0) {
        if (key == ' ') {
          if (isWalking) {
            gaitManager->stop();
            isWalking = false;
            cout << "stop walking" << endl;
            wait(200);
          } else {
            gaitManager->start();
            isWalking = true;
            cout << "start walking" << endl;
            wait(200);
          }
        }
      }

      gaitManager->step(timeStep);
      robot->step(timeStep);
    }

    return 0;
}

void wait(int ms) {
    double startTime = robot->getTime();
    double s = (double)ms / 1000.0;
    while(s + startTime >= robot->getTime())
        robot->step(timeStep);
}

void checkIfFallen() {
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

double getDirection(double x, double y) {
  double rad = atan2(x, y);
  rad *= 180.0/M_PI;
  return rad;
}

int smoothValue(double value, double min_input, double max_input, double min_output, double max_output) {
  double clamp_value = value;
  if (value < min_input) clamp_value = min_input;
  if (value > max_input) clamp_value = max_output;
  cout << "clamp: " << clamp_value*(max_output-min_output)/(max_input-min_input) << endl
       << "min output" << min_output << endl; 
  return min_output + (clamp_value-min_input)*(max_output-min_output)/(max_input-min_input);
}