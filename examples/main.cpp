#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <numeric>
#include <cmath>
#include "HurstExponent.h"

// --- Helper: generate random walk of length n ---
std::vector<double> generateRandomWalk(int n, int seed = 42){
    std::mt19937 rng(seed);
    std::normal_distribution<double> dist(0.0, 1.0);
    std::vector<double> series;
    series.reserve(n);
    double price = 100.0;
    for(int i = 0; i < n; i++){
        price *= std::exp(dist(rng));
        series.push_back(price);
    }
    return series;
}

// --- Helper: measure single compute() call in microseconds ---
double measureOnce(std::vector<double>& series){
    HurstExponent<std::vector<double>> h(series, "geometric", true);
    auto start = std::chrono::high_resolution_clock::now();
    h.compute();
    auto end   = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

int main(){

    // -------------------------------------------------------
    // Test 1: Scaling — runtime vs series length
    // -------------------------------------------------------
    std::cout << "=== Test 1: Scaling (runtime vs N) ===\n";
    std::cout << "N\t\tMean(us)\tMin(us)\t\tMax(us)\n";

    std::vector<int> lengths = {500, 1000, 2000, 5000, 10000, 50000};
    const int REPS = 10;

    for(int n : lengths){
        std::vector<double> times;
        for(int r = 0; r < REPS; r++){
            auto series = generateRandomWalk(n, r);
            times.push_back(measureOnce(series));
        }
        double mean = std::accumulate(times.begin(), times.end(), 0.0) / REPS;
        double mn   = *std::min_element(times.begin(), times.end());
        double mx   = *std::max_element(times.begin(), times.end());
        std::cout << n << "\t\t" << mean << "\t\t" << mn << "\t\t" << mx << "\n";
    }

    // -------------------------------------------------------
    // Test 2: Thread efficiency — threaded vs sequential
    // -------------------------------------------------------
    std::cout << "\n=== Test 2: Thread overhead (N=5000, " << REPS << " reps) ===\n";
    {
        auto series = generateRandomWalk(5000);
        double total = 0.0;
        for(int r = 0; r < REPS; r++)
            total += measureOnce(series);
        std::cout << "Threaded mean: " << total / REPS << " us\n";
        std::cout << "(To compare: swap computeRs to sequential and rerun)\n";
    }

    // -------------------------------------------------------
    // Test 3: Statistical bias — mean H over 1000 random walks
    // -------------------------------------------------------
    std::cout << "\n=== Test 3: Statistical bias (1000 random walks, N=1000) ===\n";
    {
        const int SIMS = 1000;
        std::vector<double> Hvals;
        Hvals.reserve(SIMS);

        for(int r = 0; r < SIMS; r++){
            auto series = generateRandomWalk(1000, r);
            HurstExponent<std::vector<double>> h(series, "geometric", true);
            h.compute();
            double H = h.getH();
            if(H > 0.0 && H < 1.0)   // discard degenerate estimates
                Hvals.push_back(H);
        }

        double mean = std::accumulate(Hvals.begin(), Hvals.end(), 0.0) / Hvals.size();

        double sq_sum = 0.0;
        for(double v : Hvals) sq_sum += (v - mean) * (v - mean);
        double stddev = std::sqrt(sq_sum / Hvals.size());

        double bias = mean - 0.5;

        std::cout << "Simulations:  " << Hvals.size() << " / " << SIMS << " valid\n";
        std::cout << "Mean H:       " << mean    << " (ideal: 0.5)\n";
        std::cout << "Std H:        " << stddev  << " (lower is better)\n";
        std::cout << "Bias:         " << bias    << " (ideal: 0.0)\n";
    }

    std::cout << "\nDone.\n";
    return 0;
}