


template <typename Vector, typename Matrix>

class OLS{
    public:
        OLS() = delete;
        OLS(Vector y, Matrix x) : y_(y), x_(x) {}

        void solve(){
            
            
            addConstant(x);
            size_t size = std::size(x);

            double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;

            for(size_t i{} ; i < size ; ++i){
                sumX += x[i];
                sumY += y[i];
                sumXY += x[i] * y[i];
                sumX2 += x[i] * x[i];
            }

            double denominator = size*sumX2 - (sumX * sumX);

            if (denominator == 0){
                std::cerr << "Error : division by 0. X values cannot all be the same." << std::endl;
            }

            slope = (size*sumXY - sumX * sumY) / denominator;
            intercept = (sumY - slope * sumX) / size;
            
        }

        void addConstant(Matrix x){
            // need to complete this 
        }
        

    private:
        Vector y_;
        Matrix x_;

        double slope;
        double intercept;
};