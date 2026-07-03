#include <iostream>
#include <type_traits>
#include <cmath>
#include <vector>
#include <thread>
#include <numeric>
#include <algorithm>
#include <string>

template<typename Series>

class HurstExponent{
    public:
        HurstExponent() : kind(""), H(0.0), c(0.0), isSimplified(false){}
        HurstExponent(Series& timeSeries, std::string k, bool isSimplified) : timeSeries(timeSeries), isSimplified(isSimplified), H(0.0), c(0.0){
            if(k == "geometric"){
                kind = k;
            }
            else{
                std::cerr << "Kind must be geometric or change." << "\n";
            }
        };

        
        void compute(){ 
            if(!kind.empty()){
                compute(timeSeries, isSimplified, kind); 
            }
            else std::cerr << "No data input." << "\n";
        }

    
        void compute(Series& t, bool s, std::string k){
            timeSeries = t;
            isSimplified = s;
            kind = k;

            // 1. convert data to log
            std::vector<double> returns = computeLogReturns();

            // 2. generate window sizes
            std::vector<unsigned int> tau = generateTau(t.size());

            // 3. For each window size, compute one mean R/S value
            std::vector<double> RS = computeRs(tau, returns);

            std::vector<unsigned int> cleanTau;
            std::vector<double> cleanRS;
            for(size_t i{}; i < RS.size(); ++i){
                if(RS[i] > 0.0){
                    cleanTau.push_back(tau[i]);
                    cleanRS.push_back(RS[i]);
                }
            }

            // 4. Log transform
            std::vector<double> logTau = computeLog(cleanTau);
            std::vector<double> logRs = computeLog(cleanRS);

            // 5. OLS Regression
            const double sumX = std::accumulate(logTau.begin(), logTau.end(), 0.0);
            const double sumY = std::accumulate(logRs.begin(), logRs.end(), 0.0);
            const double sumXX = std::inner_product(logTau.begin(), logTau.end(), logTau.begin(), 0.0);
            const double sumXY = std::inner_product(logTau.begin(), logTau.end(), logRs.begin(), 0.0);

            const int n = logTau.size();
            const double denom = n*sumXX - pow(sumX, 2);
            H = (n*sumXY - sumX * sumY)/denom;
            const double intercept = (sumY - H*sumX) / n;
            c = std::exp(intercept);
            
        }

        double getH(){ return H; }
        double getC(){ return c; }

    protected:
        
        std::vector<double> computeLogReturns(){
            std::vector<double> r;
            for(size_t i{1} ; i < timeSeries.size() ; ++i){
                r.push_back(std::log(timeSeries[i] / timeSeries[i-1]));
            }
            return r;
        }

        template<typename T>
        std::vector<double> computeLog(const std::vector<T>& v){
            std::vector<double> r;
            for(size_t i{0} ; i < v.size() ; ++i){
                r.push_back(std::log(v[i]));
            }
            return r;
        }
        
        std::vector<unsigned int> generateTau(size_t size){
            const unsigned short min = 10;
            const unsigned max = size/4;
            const unsigned nWindows = 20;

            std::vector<unsigned int> tau;

            for(size_t i{} ; i < nWindows ; ++i){

                double s = std::pow(10.0, std::log((double)min) + i*(std::log((double)max) - std::log((double)min)) / (nWindows-1));
                unsigned int t = static_cast<unsigned int>(std::round(s));

                if(tau.empty() || t != tau.back()){
                    tau.push_back(t);
                }
            }

            return tau;

        }



        std::vector<double> computeRs(const std::vector<unsigned int>& tau, const std::vector<double>& returns){
           
            std::vector<double> RS(tau.size());

            auto worker = [&](int idx){ RS[idx] = computeWindowRs(returns, tau[idx]); };

            std::vector<std::thread> threads;
            for(size_t i{} ; i < tau.size() ; ++i){
                threads.emplace_back(worker, i);
            }

            for(size_t i{} ; i < tau.size() ; ++i){
                threads[i].join();
            }
            return RS;
        }

        double computeWindowRs(const std::vector<double>& returns, const unsigned int window){
            size_t n = returns.size();
            unsigned int numChunks = n / window;
            std::vector<double> rsChunks;

            for (int chunk = 0; chunk < numChunks; chunk++) {
                unsigned int start = chunk * window;

                // mean of chunk
                double m = 0.0;
                for (size_t i{start}; i < start + window; i++)
                    m += returns[i];
                m /= window;

                // cumulative sum of mean-centered chunk
                std::vector<double> dev(window);
                double running = 0.0;
                for (size_t i{}; i < window; i++) {
                    running += (returns[start + i] - m);
                    dev[i] = running;
                }
            
                // R = range of deviation profile
                double R = *std::max_element(dev.begin(), dev.end()) - *std::min_element(dev.begin(), dev.end());
            
                // S = std dev of raw chunk
                double sqSum = 0.0;
                for (size_t i{start}; i < start + window; i++)
                    sqSum += (returns[i] - m) * (returns[i] - m);
                double S = std::sqrt(sqSum / (double)window);
            
                // store R/S
                if (S > 0.0)
                    rsChunks.push_back(R / S);
            }

            double sum = 0.0;
            for (double v : rsChunks) sum += v;
            return sum / rsChunks.size();
        }
    

        

    
        


    private:
        
        // Algo parameters
        Series timeSeries;
        std::string kind;
        bool isSimplified;
        
        // Resulting 
        double H;
        double c;

      
};
