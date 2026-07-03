#include <cmath>

class JarqueBera{

    public:
        JarqueBera() = default;
        double operator()(unsigned int n, double skewness, double kurtosis);


}

double JarqueBera::operator()(unsigned int n, double skewness, double kurtosis){
    return (static_cast<double>(n)/6)*(pow(skewness, 2) + (pow(kurtosis-3, 2)/4));
}