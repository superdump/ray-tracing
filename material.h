#ifndef MATERIALH
#define MATERIALH

#include "hitable.h"
#include "ray.h"
#include "texture.h"
#include "vec3.h"

float schlick(float cosine, float ref_idx) {
    float r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
    r0 *= r0;
    return r0 + (1.0f - r0) * powf((1.0f - cosine), 5.0f);
}

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
    vec3 uv = unit_vector(v);
    float dt = dot(uv, n);
    float discriminant = 1.0f - ni_over_nt * ni_over_nt * (1.0f - dt * dt);
    if (discriminant > 0.0f) {
        refracted = ni_over_nt * (uv - n * dt) - n * sqrtf(discriminant);
        return true;
    }
    return false;
}

vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2.0f * dot(v, n) * n;
}

vec3 random_in_unit_sphere() {
    vec3 p;
    do {
        p = 2.0f * vec3(r01(rng), r01(rng), r01(rng)) - ones;
    } while (p.squared_length() >= 1.0f);
    return p;
}

class material {
public:
    virtual bool scatter(const ray& r_in,
                         const hit_record& rec,
                         vec3& attenuation,
                         ray& scattered) const = 0;
    virtual vec3 emitted(float u, float v, const vec3& p) const {
        return vec3(0.0f, 0.0f, 0.0f);
    }
};

class diffuse_light : public material {
public:
    diffuse_light(texture *a) : emit(a) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
        return false;
    }
    virtual vec3 emitted(float u, float v, const vec3& p) const {
        return emit->value(u, v, p);
    }

    texture *emit;
};

class isotropic : public material {
public:
    isotropic(texture *a) : albedo(a) {}

    virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const {
        scattered = ray(rec.p, random_in_unit_sphere());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

    texture *albedo;
};

class lambertian : public material {
public:
    lambertian(texture *a) : albedo(a) {}

    virtual bool scatter(const ray& r_in,
                         const hit_record& rec,
                         vec3& attenuation,
                         ray& scattered) const {
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        scattered = ray(rec.p, target - rec.p, r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

    texture *albedo;
};

class metal : public material {
public:
    metal(const vec3& a, float f) : albedo(a) {
        if (f < 1.0f) {
            fuzz = f;
        } else {
            fuzz = 1.0f;
        }
    }

    virtual bool scatter(const ray& r_in,
                         const hit_record& rec,
                         vec3& attenuation,
                         ray& scattered) const {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.time());
        attenuation = albedo;
        return dot(scattered.direction(), rec.normal) > 0.0f;
    }

    vec3 albedo;
    float fuzz;
};

class dielectric : public material {
public:
    dielectric(float ri) : ref_idx(ri) {}

    virtual bool scatter(const ray& r_in,
                         const hit_record& rec,
                         vec3& attenuation,
                         ray& scattered) const {
        vec3 outward_normal;
        float ni_over_nt;
        attenuation = ones;
        vec3 refracted;
        float reflect_prob;
        float cosine;
        if (dot(r_in.direction(), rec.normal) > 0.0f) {
            outward_normal = -rec.normal;
            ni_over_nt = ref_idx;
            cosine = ref_idx + dot(r_in.direction(), rec.normal) / r_in.direction().length();
        } else {
            outward_normal = rec.normal;
            ni_over_nt = 1.0f / ref_idx;
            cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }
        reflect_prob = refract(r_in.direction(), outward_normal, ni_over_nt, refracted)
            ? schlick(cosine, ref_idx)
            : 1.0f;
        if (r01(rng) < reflect_prob) {
            vec3 reflected = reflect(r_in.direction(), rec.normal);
            scattered = ray(rec.p, reflected, r_in.time());
        } else {
            scattered = ray(rec.p, refracted, r_in.time());
        }
        return true;
    }

    float ref_idx;
};

#endif /* MATERIALH */
