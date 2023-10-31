#include <QtWidgets/QApplication>
#include <iostream>

#include "main_window.hpp"

using namespace std;

int main(int argc, char** argv) {
  QApplication *app = new QApplication(argc, argv);
  GlobalData *global = new GlobalData("../");
  MainWindow *window = new MainWindow(global);

  window->show();
  return app->exec();
}