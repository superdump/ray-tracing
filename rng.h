#ifndef RNGH
#define RNGH

#include <random>

std::random_device rd;
std::mt19937 rng(rd());
std::uniform_real_distribution<float> r01(0.0f, 1.0f);

static uint32_t XorShift32(uint32_t& state) {
    uint32_t x = state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 15;
    state = x;
    return x;
}

float RandomFloat01(uint32_t& state) {
    return (XorShift32(state) & 0xFFFFFF) / 16777216.0f;
}

#endif /* RNGH */
