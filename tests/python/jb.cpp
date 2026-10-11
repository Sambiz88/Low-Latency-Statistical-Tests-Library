#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include "../../include/JarqueBera.h"

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
    auto start = std::chrono::high_resolution_clock::now();
    const std::vector<double> values = readCsvColumn("spy.csv", 1);
    JarqueBera jb;
    jb(values);
    double jbStat = jb.getJbStat();
    double pValue = jb.getPValue();

    std::cout << "Jarque-Bera Statistic: " << jbStat << '\n';
    std::cout << "P-value: " << pValue << '\n';
    if(pValue < 0.05){
        std::cout << "Reject the null hypothesis: the data is not normally distributed" << std::endl;
    }
    else{
        std::cout << "Fail to reject the null hypothesis: the data is normally distributed" << std::endl;
    }


    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Execution time: " << duration.count() << " ms" << std::endl;

    return 0;
}
