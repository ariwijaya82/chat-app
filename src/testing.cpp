#include "monitoring.hpp"

int main(int argc, char** argv) {
    QApplication* app = new QApplication(argc, argv);
    Monitoring* monitor = new Monitoring();
    monitor->show();
    return app->exec();
}