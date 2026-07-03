#include <cmath>
#include <iterator>
#include <type_traits>


template <typename Container>
class DurbinWatson {
    public:
        DurbinWatson() = default;
        double operator()(const Container& c);
};



template <typename Container>
double DurbinWatson<Container>::operator()(const Container& c){

    using T = typename Container::value_type;
    static_assert(std::is_arithmetic<T>::value, "Container must contain numeric values");

    double numerator = 0.0;
    double denominator = 0.0;

    for(auto e = std::next(std::begin(c)); e != std::end(c); ++e){
        numerator += std::pow(*e - *std::prev(e), 2);
    }

    for(auto e = std::begin(c); e != std::end(c);++e){
        denominator += std::pow(*e, 2);
    }

    return numerator/denominator;

    
}