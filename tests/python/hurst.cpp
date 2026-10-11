#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "../../include/HurstExponent.h"

std::vector<double> readCsvColumn(const std::string& filename,
                                  std::size_t columnIndex = 0) {
    std::ifstream file(filename);
    std::string line;
    std::vector<double> values;

    std::getline(file, line);
    while (std::getline(file, line)) {
        std::stringstream stream(line);
        std::string cell;

        for (std::size_t column = 0; column <= columnIndex; ++column) {
            if (!std::getline(stream, cell, ',')) {
                break;
            }
        }

        if (!cell.empty()) {
            values.push_back(std::stod(cell));
        }
    }

    return values;
}

int main() {
    const std::vector<double> values = readCsvColumn("spy.csv", 1);
    HurstExponent hurst = {values, "geometric", true};
    hurst.compute();
    double H = hurst.getH();

    std::cout << "H exponent: " << H << std::endl;
    return 0;
}
