#include <boost/math/distributions/chi_squared.hpp>
#include <numeric>


template <typename Container>
Eigen::MatrixX4d LjungBox(const Container& residuals, const unsigned int maxLags, const unsigned int df){

    const size_t size = std::distance(std::begin(residuals), std::end(residuals));
    
    // Allocate matrix exactly for the number of lags requested
    Eigen::MatrixX4d mat(maxLags, 4);
    if (size == 0 || maxLags == 0) return mat;

    // 1. Calculate sample mean
    double mean = std::accumulate(std::begin(residuals), std::end(residuals), 0.0) / static_cast<double>(size);

    // 2. Compute Sample Autocorrelations
    double denominator = 0.0;
    for (const auto& e : residuals) {
        denominator += std::pow(e - mean, 2);
    }
    
    for (size_t k = 1; k <= maxLags; ++k) {
        double numerator = 0.0;
        // Corrected 0-based indexing boundary
        for (size_t t = k; t < size; ++t) {
            numerator += (residuals[t] - mean) * (residuals[t - k] - mean);
        }
        double rho = numerator / denominator;
        mat(k - 1, 0) = static_cast<double>(k);
        mat(k - 1, 1) = rho;
    }

    // 3. Cumulative Loop
    double cumSum = 0.0;
    
    for (size_t h = 1; h <= maxLags; ++h) {
        // Corrected matrix access: row is h-1, col is 1
        double rho = mat(h - 1, 1); 
        cumSum += (rho * rho) / static_cast<double>(size - h);
        
        double Qh = static_cast<double>(size * (size + 2)) * cumSum;
        int dof = static_cast<int>(h) - static_cast<int>(df);
        
        double p = std::numeric_limits<double>::quiet_NaN(); 
        
        // p-value is only valid if degrees of freedom > 0
        if (dof > 0) {
            boost::math::chi_squared dist(dof);
            // Survival function / complement of CDF for upper tail area
            p = boost::math::cdf(boost::math::complement(dist, Qh));
        }

        mat(h - 1, 2) = Qh;
        mat(h - 1, 3) = p; // Fixed typo 'math' -> 'mat'
    }

    return mat;

};