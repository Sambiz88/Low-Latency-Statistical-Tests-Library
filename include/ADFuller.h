#include <vector>
#include <cmath>
#include "OLS.h"

template <typename T>
class ADFuller{

    private:
        double testStatistic;
        double pValue;
        int lags;
        int maxLag;
        const T& series;
        bool isAutoLag;

    public:
        ADFuller() : isAutoLag(true) {}
        ADFuller(const T& series) : series(series), isAutoLag(true) {}
        // ADFuller(const T& series, int maxLag) : series(series), maxLag(maxLag), isAutoLag(false) {}

        double getTestStatistic() { return testStatistic; }
        double getPValue() { return pValue; }
        int getLags() { return lags; }
        void setIsAutoLag(bool b) { isAutoLag = b; }
        
        double run(){
            // Compute first differences

            std::vector<double> deltaY;
            size_t size = deltaY.size();

            for(size_t i{1} ; i < size ; ++i){
                deltaY.push_back(series[i] - series[i-1])
            }

            // 2. Max lag upper bound
            maxLag = floor(12 * pow(size/100, 1.0/4.0));

            // 3. Select optimal lag length
            float lowestAic = INFINITY;
            int kForLowestAic = 0;
            
            for(size_t p{} ; p <= maxLag ; ++p){

                // 3.1 Build X for OLS
                const int n = size-p-1;
                const int k = p+2;
                std::vector<std::vector<float>> X(n, std::vector<float>(k));
                std::vector<float> y(n);
                
                for(size_t i{} ; i < n ; ++i){
                    const size_t t = i+p+1;
                    X[i][0] = 1.0;
                    X[i][1] = series[t-1];
                    y[i] = series[t] - series[t-1];

                    for(size_t lag{1} ; lag <= p ; ++lag){
                        X[i][1+lag] = series[t-lag] - series[t-lag-1];
                    }
                }

                OLS ols(X, y);
                const float Aic = (size*log(ols.getRss()/size)) + 2*(k+2);
                if(lowestAic > Aic){
                    lowestAic = Aic;
                    kForLowestAic = Aic;
                }
            
            }
  
            int p = kForLowestAic - 2;
            int n = size-p-1;
            std::vector<std::vector<float>> X(n, std::vector<float>(kForLowestAic));
            std::vector<float> y(n);

            for(size_t i{} ; i < n ; ++i){
                    const size_t t = i+p+1;
                    X[i][0] = 1.0;
                    X[i][1] = series[t-1];
                    y[i] = series[t] - series[t-1];

                    for(size_t lag{1} ; lag <= p ; ++lag){
                        X[i][1+lag] = series[t-lag] - series[t-lag-1];
                    }
                }

            OLS ols(X, y);

            ols.get


        } 
    

};

