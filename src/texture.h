#ifndef TEXTUREH
#define TEXTUREH

#include "perlin.h"
#include "vec3.h"

class texture {
public:
    virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

class constant_texture : public texture {
public:
    constant_texture() {}
    constant_texture(vec3 c) : color(c) {}

    virtual vec3 value(float u, float v, const vec3 &p) const {
        return color;
    }

    vec3 color;
};

class checker_texture : public texture {
public:
    checker_texture() {}
    checker_texture(texture *t0, texture *t1) : odd(t1), even(t0) {}

    virtual vec3 value(float u, float v, const vec3 &p) const {
        float sines = sinf(10.0f * p.x()) * sinf(10.0f * p.y()) * sinf(10.0f * p.z());
        return sines < 0.0f ? odd->value(u, v, p) : even->value(u, v, p);
    }

    texture *odd;
    texture *even;
};

class noise_texture : public texture {
public:
    noise_texture() {}
    noise_texture(float scl) : scale(scl) {}

    virtual vec3 value(float u, float v, const vec3& p) const {
        // return vec3(1, 1, 1) * 0.5f * (1.0f + noise.noise(scale * p));
        // return vec3(1, 1, 1) * noise.turb(scale * p);
        return vec3(1, 1, 1) * 0.5f * (1.0f + sinf(scale * p.x() + 5.0f * noise.turb(scale * p)));
    }

    perlin noise;
    float scale;
};

#endif /* TEXTUREH */
