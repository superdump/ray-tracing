#ifndef MATERIAL_HH
#define MATERIAL_HH

#include "hitable.hh"
#include "ray.hh"
#include "vec3.hh"

class material {
public:
    virtual bool scatter(const ray& r_in,
                         const hit_record& rec,
                         vec3& attenuation,
                         ray& scattered) const = 0;
};

class lambertian : public material {
public:
    vec3 albedo;

    lambertian(const vec3& a) : albedo(a) {}

    virtual bool scatter(const ray& r_in,
                         const hit_record& rec,
                         vec3& attenuation,
                         ray& scattered) const {
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        scattered = ray(rec.p, target - rec.p);
        attenuation = albedo;
        return true;
    }
};

class metal : public material {
public:
    vec3 albedo;
    float fuzz;

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
        scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
        attenuation = albedo;
        return dot(scattered.direction(), rec.normal) > 0.0f;
    }
};

#endif /* MATERIAL_HH */
