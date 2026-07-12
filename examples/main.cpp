#include "OLS.h"
#include <iostream>
#include <vector>
#include <array>

int main(){

    std::vector<std::vector<double>> matrix;
    matrix.push_back({1., 2.});
    matrix.push_back({4., 5.});
    matrix.push_back({5., 9.});
    matrix.push_back({6., 5.});
    matrix.push_back({7., 5.});

    std::array<double, 5> y = {7., 8., 9., 10., 11.};
 
    OLS ols{matrix, y};

    std::cout << ols.fitted;
}