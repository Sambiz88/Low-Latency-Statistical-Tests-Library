#pragma once

#include <iostream>
#include <vector>

#include "ADFuller.h"
template <typename T>

class EngleGranger{
     
public:
    EngleGranger() = delete;
    EngleGranger(const T& series1, const T& series2) : series1(series1), series2(series2), isAutoLag(true) {
            run();
    }

    
protected:
    void run(){
        if (ADFuller(series1).getPValue() < 0.05){
            std::cout << "Series 1 is stationary" << std::endl;
        };

        if (ADFuller(series2).getPValue() < 0.05){
            std::cout << "Series 2 is stationary" << std::endl;
        };

        
        std::vector<float> deltaS1;
        std::transform(series1.begin()+1, series1.end(), series1.begin(), deltaS1.begin(), [](float curr, float prev) { return curr-prev; });

        std::vector<float> deltaS2;
        std::transform(series2.begin()+1, series2.end(), series1.begin(), deltaS1.begin(), [](float curr, float prev) { return curr - prev; });

        if (ADFuller)
        OLS ols(series1, series2);

        const T& residuals = ols.getResiduals();

    
        
    }

private:
    const T& series1;
    const T& series2;

};

/*
input 1: two non stationary time series (one dependent and the other not) + model spec
outputs 1 + input 2 : redisuals series
output 2: cointegration verdict
*/