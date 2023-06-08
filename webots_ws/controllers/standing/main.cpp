#include <webots/Robot.hpp>
#include <webots/PositionSensor.hpp>
#include <RobotisOp2MotionManager.hpp>

#include <string>

using namespace webots;
using namespace managers;
using namespace std;

static const char *motorNames[20] = {
  "ShoulderR" /*ID1 */, "ShoulderL" /*ID2 */, "ArmUpperR" /*ID3 */, "ArmUpperL" /*ID4 */, "ArmLowerR" /*ID5 */,
  "ArmLowerL" /*ID6 */, "PelvYR" /*ID7 */,    "PelvYL" /*ID8 */,    "PelvR" /*ID9 */,     "PelvL" /*ID10*/,
  "LegUpperR" /*ID11*/, "LegUpperL" /*ID12*/, "LegLowerR" /*ID13*/, "LegLowerL" /*ID14*/, "AnkleR" /*ID15*/,
  "AnkleL" /*ID16*/,    "FootR" /*ID17*/,     "FootL" /*ID18*/,     "Neck" /*ID19*/,      "Head" /*ID20*/
};

int main() {
    webots::Robot* robot = new webots::Robot();
    double timeStep = robot->getBasicTimeStep();

    PositionSensor* positionSensors[20];
    for (int i = 0; i < 20; i++) {
        string sensorName = motorNames[i];
        sensorName.push_back('S');
        positionSensors[i] = robot->getPositionSensor(sensorName);
        positionSensors[i]->enable(timeStep);
    }

    RobotisOp2MotionManager* motion = new RobotisOp2MotionManager(robot);
    robot->step(timeStep);
    motion->playPage(9);
    double startTime = robot->getTime();
    while (startTime + 2000 > robot->getTime())
        robot->step(timeStep);

    delete robot;
    delete motion;
    return 0;
}