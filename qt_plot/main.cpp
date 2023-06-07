#include "plot.hpp"
#include <iostream>
#include <chrono>

using namespace std;

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    RealTimePlot plot;
    plot.show();
    auto start_time = chrono::steady_clock::now();
    for (int i = 0; i < 1000; i++) {
        // cout << i << endl;
        plot.addData(i, i);
        // int temp = 0;
        auto delta = chrono::steady_clock::now() - start_time;
        while (chrono::duration_cast<chrono::milliseconds>(delta).count() < i * 1000) {
            delta = chrono::steady_clock::now() - start_time;
        }
        cout << i << endl;
        app.processEvents();
    }

    // return app.exec();
}