#include "path_generator.hpp"
#include "monitor.hpp"
#include "utils.hpp"

int main(int argc, char** argv) {
    QApplication* app = new QApplication(argc, argv);
    Monitor* monitor = new Monitor();
    monitor->setDisableButton(true);
    monitor->show();

    return app->exec();
}