#include <cmath>
#include <stdexcept>
#include <vector>

class JarqueBera{

    public:
        JarqueBera() : jbStat(0.0), pValue(1.0) {}
        double operator()(unsigned int n, double skewness, double kurtosis);
        void operator()(const std::vector<double>& values);
        double getPValue(){
            if (jbStat < 0.0) return 1.0; // Statistical scores cannot be negative
            double df = 2.0; 
            return std::exp(-jbStat / 2.0);
        }
    
        double getJbStat(){
            return jbStat;
        }
    private:
        double jbStat;
        double pValue;
};

double JarqueBera::operator()(unsigned int n, double skewness, double kurtosis){
    jbStat = (static_cast<double>(n)/6)*(pow(skewness, 2) + (pow(kurtosis-3, 2)/4));
    return jbStat;
}

void JarqueBera::operator()(const std::vector<double>& values){
    if (values.empty()) {
        throw std::invalid_argument("JarqueBera requires at least one value");
    }

    double mean = 0.0;
    for (double value : values) {
        mean += value;
    }
    mean /= static_cast<double>(values.size());

    // to compute skeweness and kurtosis
    double secondMoment = 0.0;
    double thirdMoment = 0.0;
    double fourthMoment = 0.0;
    for (double value : values) {
        const double deviation = value - mean;
        const double squaredDeviation = deviation * deviation;
        secondMoment += squaredDeviation;
        thirdMoment += squaredDeviation * deviation;
        fourthMoment += squaredDeviation * squaredDeviation;
    }

    const double n = static_cast<double>(values.size());
    secondMoment /= n;
    if (secondMoment == 0.0) {
        throw std::invalid_argument("JarqueBera requires non-zero variance");
    }

    const double skewness = (thirdMoment / n) / std::pow(secondMoment, 1.5);
    const double kurtosis = (fourthMoment / n) / (secondMoment * secondMoment);
    jbStat = (*this)(static_cast<unsigned int>(values.size()), skewness, kurtosis);
}