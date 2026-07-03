#include <type_traits

template <typename Container>
class ChowTest{
    public:
        ChowTest() = default;
        bool operator()(Container y, Container X, unsigned int lastIndexInModel1, unsigned int firstIndexModel2 , double sigLevel);

    private:
        double fStatistic;
        double pValue;
        
};

bool ChowTest::operator()(Container y, Container X, unsigned int lastIndexInModel1, unsigned int firstIndexModel2 , double sigLevel){

    using T = typename Container::value_type;
    static_assert(std::is_arithmetic<T>::value, "Container must contain numeric values");

    if constexpr (std::is_same_v<Container, std::vector<T>>){
        // vector code
        // check how to convert matrix of vector of vector 
        Eigen::Map<Eigen::Vector3d> y(y.data());

        Eigen::
    }
    else if constexpr (std::is_array_v<Container>){
        // c code
    }

    // else if constexpr (std::is_same_v<Container, std::deque<T>>){
    //     std::vector<T> v(y.begin())
    // }


}