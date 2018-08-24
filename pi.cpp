#include <cmath>
#include <iostream>
#include <random>

int main(int argc, char const *argv[]) {
    int sqrt_N = 10000;
    int inside_circle = 0;
    int inside_circle_stratified = 0;

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<float> r01(0.0f, 1.0f);

    for (int i = 0; i < sqrt_N; ++i) {
        for (int j = 0; j < sqrt_N; ++j) {
            float x = 2.0f * r01(rng) - 1.0f;
            float y = 2.0f * r01(rng) - 1.0f;
            if ( x * x + y * y < 1.0f) {
                ++inside_circle;
            }
            x = 2.0f * ((i + r01(rng)) / sqrt_N) - 1;
            y = 2.0f * ((j + r01(rng)) / sqrt_N) - 1;
            if ( x * x + y * y < 1.0f) {
                ++inside_circle_stratified;
            }
        }
    }

    std::cout << "Regular estimate of Pi = " << 4.0f * float(inside_circle) / float(sqrt_N * sqrt_N) << "\n";
    std::cout << "Stratified estimate of Pi = " << 4.0f * float(inside_circle_stratified) / float(sqrt_N * sqrt_N) << "\n";
    return 0;
}
