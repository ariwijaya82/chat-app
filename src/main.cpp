#include "path_generator.hpp"
#include "monitor.hpp"
#include "utils.hpp"

int main(int argc, char** argv) {
  GlobalData *global = new GlobalData("../");

  QApplication* app = new QApplication(argc, argv);
  Monitor* monitor = new Monitor(global);
  monitor->show();

  return app->exec();
}