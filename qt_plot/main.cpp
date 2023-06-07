#include "plot.hpp"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    RealTimePlot plot;
    plot.show();

    return app.exec();
}