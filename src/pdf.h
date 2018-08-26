#ifndef PDFH
#define PDFH

#include <cmath>

#include "onb.h"
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

class pdf {
public:
    virtual float value(const vec3& direction) const = 0;
    virtual vec3 generate(uint32_t& state) const = 0;
};

class cosine_pdf : public pdf {
public:
    cosine_pdf(const vec3& w) { uvw.build_from_w(w); }
    virtual float value(const vec3& direction) const {
        float cosine = dot(unit_vector(direction), uvw.w());
        if (cosine > 0.0f) {
            return cosine / M_PI;
        } else {
            return 0.0f;
        }
    }
    virtual vec3 generate(uint32_t& state) const {
        return uvw.local(random_cosine_direction(state));
    }
    onb uvw;
};

class hitable_pdf : public pdf {
public:
    hitable_pdf(hitable *p, const vec3& origin) : ptr(p), o(origin) {}
    virtual float value(const vec3& direction) const {
        return ptr->pdf_value(o, direction);
    }
    virtual vec3 generate(uint32_t& state) const {
        return ptr->random(o);
    }
    vec3 o;
    hitable *ptr;
};

class mixture_pdf : public pdf {
public:
    mixture_pdf(pdf *p0, pdf *p1) { p[0] = p0; p[1] = p1; }
    virtual float value(const vec3& direction) const {
        return 0.5f * p[0]->value(direction) + 0.5f * p[1]->value(direction);
    }
    virtual vec3 generate(uint32_t& state) const {
        if (RandomFloat01(state) < 0.5f) {
            return p[0]->generate(state);
        } else {
            return p[1]->generate(state);
        }
    }
    pdf *p[2];
};

#endif // PDFH
