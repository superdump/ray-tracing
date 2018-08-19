#ifndef RNGH
#define RNGH

#include <random>

std::random_device rd;
std::mt19937 rng(rd());
std::uniform_real_distribution<float> r01(0.0f, 1.0f);

#endif /* RNGH */
