#ifndef MATERIALH
#define MATERIALH

#include "hitable.h"
#include "onb.h"
#include "pdf.h"
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

vec3 random_in_unit_sphere(uint32_t& state) {
    vec3 p;
    do {
        p = 2.0f * vec3(RandomFloat01(state), RandomFloat01(state), RandomFloat01(state)) - ones;
    } while (p.squared_length() >= 1.0f);
    return p;
}

struct scatter_record {
    ray specular_ray;
    bool is_specular;
    vec3 attenuation;
    pdf *pdf_ptr;
};

class material {
public:
    virtual bool scatter(const ray& r_in,
                         const hit_record& hrec,
                         scatter_record& srec,
                         uint32_t& state) const {
        return false;
    }
    virtual float scattering_pdf(const ray& r_in,
                                 const hit_record& rec,
                                 const ray& scattered) const {
        return 0.0f;
    }
    virtual vec3 emitted(const ray& r_in, const hit_record& rec, float u, float v, const vec3& p) const {
        return vec3(0.0f, 0.0f, 0.0f);
    }
};

class diffuse_light : public material {
public:
    diffuse_light(texture *a) : emit(a) {}

    virtual bool scatter(const ray& r_in,
                         const hit_record& rec,
                         vec3& attenuation,
                         ray& scattered,
                         uint32_t& state) const {
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

    virtual bool scatter(const ray &r_in,
                         const hit_record &rec,
                         vec3 &attenuation,
                         ray &scattered,
                         uint32_t& state) const {
        scattered = ray(rec.p, random_in_unit_sphere(state));
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

    texture *albedo;
};

class lambertian : public material {
public:
    lambertian(texture *a) : albedo(a) {}

    virtual float scattering_pdf(const ray& r_in,
                                 const hit_record& rec,
                                 ray& scattered,
                                 uint32_t& state) const {
        float cosine = dot(rec.normal, unit_vector(scattered.direction()));
        if (cosine < 0.0f) {
            cosine = 0.0f;
        }
        return cosine / M_PI;
    }
    virtual bool scatter(const ray& r_in,
                         const hit_record& hrec,
                         scatter_record& srec,
                         uint32_t& state) const {
        srec.is_specular = false;
        srec.attenuation = albedo->value(hrec.u, hrec.v, hrec.p);
        srec.pdf_ptr = new cosine_pdf(hrec.normal);
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
                         const hit_record& hrec,
                         scatter_record& srec,
                         uint32_t& state) const {
        vec3 reflected = reflect(unit_vector(r_in.direction()), hrec.normal);
        srec.specular_ray = ray(hrec.p, reflected + fuzz * random_in_unit_sphere(state), r_in.time());
        srec.attenuation = albedo;
        srec.is_specular = true;
        srec.pdf_ptr = NULL;
        return true;
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
                         ray& scattered,
                         uint32_t& state) const {
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
        if (RandomFloat01(state) < reflect_prob) {
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
