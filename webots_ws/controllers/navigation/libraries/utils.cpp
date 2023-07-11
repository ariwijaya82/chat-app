#include "utils.hpp"

#include <fstream>
#include <sstream>

vector<vector<double>> utils::read_position_csv() {
    vector<vector<double>> result;
    std::string filename = "../../data/position.csv";
    std::fstream file(filename, std::ios::in);
    std::string line, x, y;
    if (file.is_open()) {
        while (getline(file, line)) {
            std::stringstream str(line);
            getline(str, x, ',');
            getline(str, y, ',');
            result.push_back(vector<double>{stod(x), stod(y)});    
        }
    }

    return result;
}

double utils::smoothValue(double value, double min_input, double max_input, double min_output, double max_output) {
  double clamp_value = value;
  if (value < min_input) clamp_value = min_input;
  if (value > max_input) clamp_value = max_input;
  return min_output + (clamp_value-min_input)*(max_output-min_output)/(max_input-min_input);
}