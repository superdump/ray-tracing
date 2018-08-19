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
    vec3 color;

    constant_texture() {}
    constant_texture(vec3 c) : color(c) {}

    virtual vec3 value(float u, float v, const vec3 &p) const {
        return color;
    }
};

class checker_texture : public texture {
public:
    texture *odd;
    texture *even;

    checker_texture() {}
    checker_texture(texture *t0, texture *t1) : odd(t1), even(t0) {}

    virtual vec3 value(float u, float v, const vec3 &p) const {
        float sines = sinf(10.0f * p.x()) * sinf(10.0f * p.y()) * sinf(10.0f * p.z());
        return sines < 0.0f ? odd->value(u, v, p) : even->value(u, v, p);
    }
};

class noise_texture : public texture {
public:
    perlin noise;
    float scale;

    noise_texture() {}
    noise_texture(float scl) : scale(scl) {}

    virtual vec3 value(float u, float v, const vec3& p) const {
        // return vec3(1, 1, 1) * noise.noise(scale * p);
        // return vec3(1, 1, 1) * noise.turb(scale * p);
        return vec3(1, 1, 1) * 0.5f * (1.0f + sinf(scale * p.z() + 10.0f * noise.turb(p)));
    }
};

class image_texture : public texture {
public:
    unsigned char *data;
    int nx, ny;

    image_texture() {}
    image_texture(unsigned char *pixels, int A, int B) : data(pixels), nx(A), ny(B) {}

    virtual vec3 value(float u, float v, const vec3& p) const;
};

vec3 image_texture::value(float u, float v, const vec3 &p) const {
    int i = (u    ) * nx;
    int j = (1 - v) * ny - 0.001f;
    if (i < 0) i = 0;
    if (j < 0) j = 0;
    if (i > nx - 1) i = nx - 1;
    if (j > ny - 1) j = ny - 1;
    float r = int(data[3 * (i + nx * j)    ]) / 255.0f;
    float g = int(data[3 * (i + nx * j) + 1]) / 255.0f;
    float b = int(data[3 * (i + nx * j) + 2]) / 255.0f;
    return vec3(r, g, b);
}

#endif /* TEXTUREH */
