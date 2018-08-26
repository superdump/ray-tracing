#include <cmath>
#include <iostream>

inline float pdf(float x) {
    return 0.5f * x;
}

int main() {
    int N = 1000000;
    float sum = 0.0f;
    for (int i = 0; i < N; ++i) {
        float x = sqrtf(4.0f * drand48());
        sum += x * x / pdf(x);
    }
    std::cout << "I = " << sum / N << "\n";
}
