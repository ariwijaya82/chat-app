#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <vector>

using namespace std;

namespace utils {
    vector<vector<double>> read_position_csv();
    double smoothValue(double value, double min_input, double max_input, double min_output, double max_output);
}

#endif