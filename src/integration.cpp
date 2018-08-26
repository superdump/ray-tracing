#include <cmath>
#include <iostream>

inline float pdf(float x) {
    return 3.0f * x * x / 8.0f;
}

int main() {
    int N = 1000000;
    float sum = 0.0f;
    for (int i = 0; i < N; ++i) {
        float x = powf(8.0f * drand48(), 1.0f / 3.0f);
        sum += x * x / pdf(x);
    }
    std::cout << "I = " << sum / N << "\n";
}
