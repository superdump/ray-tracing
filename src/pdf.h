#ifndef PDFH
#define PDFH

#include <cmath>

#include "rng.h"
#include "vec3.h"

inline vec3 random_cosine_direction(uint32_t& state) {
    float r1 = RandomFloat01(state);
    float r2 = RandomFloat01(state);
    float z = sqrtf(1.0f - r2);
    float phi = 2.0f * M_PI * r1;
    float two_sqrtf_r2 = 2.0f * sqrtf(r2);
    float x = cosf(phi) * two_sqrtf_r2;
    float y = sinf(phi) * two_sqrtf_r2;
    return vec3(x, y, z);
}

#endif // PDFH
