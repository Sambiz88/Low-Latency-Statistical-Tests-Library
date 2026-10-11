#include <type_traits>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <numeric>


class TTest{

public:
    TTest(){

    }


};

// Checks if T has a .size()
template <typename T, typename = void>
struct is_contiguous_and_dynamic : std::false_type{};

template <typename T>
struct is_contiguous_and_dynamic<T, std::void_t<decltype(std::declval<T>().size())>> : std::true_type{};


template <typename T>
class PairedTTest{

public:
    PairedTTest(const T& obs1, const T& obs2) : obs1(obs1), obs2(obs2), t(0.0){
        run();
    }

    void run(){
        
        assert(obs1.size() == obs2.size());
        if constexpr (is_contiguous_and_dynamic<T>::value){
            
            int n = obs1.size();
            std::vector<float> diff(n);
            std::transform(obs1.begin(), obs1.end(), obs2.begin(), diff.begin(),[&](float x, float y){return x-y;});

            float d = std::accumulate(diff.begin(), diff.end(), 0.0/n);
            float s = 0.0;

            for(const auto& x : diff){
                s += pow(x-d, 2);
            }
            s = pow(s/n-1, 0.5);
            t = d/(s/pow(n, 0.5));


        }
        else {
            std::cout << "Values must have .size()" << std::endl;
            return;
        }
    }

    float getT(){
        
    }

private:
    const T& obs1;
    const T& obs2;
    float t;
}