#ifndef SPHEREH
#define SPHEREH

#include "aabb.h"
#include "hitable.h"
#include "material.h"
#include "ray.h"
#include "vec3.h"

class sphere : public hitable {
public:
    sphere() {}
    sphere(vec3 cen, float r, material *m) : center(cen), radius(r), mat_ptr(m) {}

    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec, uint32_t& state) const;
    virtual bool bounding_box(float t0, float t1, aabb &box) const;

    vec3 center;
    float radius;
    material *mat_ptr;
};

bool sphere::bounding_box(float t0, float t1, aabb &box) const {
    vec3 half_diagonal(radius, radius, radius);
    box = aabb(center - half_diagonal, center + half_diagonal);
    return true;
}

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec, uint32_t& state) const {
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - a * c;
    if (discriminant > 0.0f) {
        float discriminantSqrt = sqrtf(discriminant);
        float temp = (-b - discriminantSqrt) / a;
        if (temp > t_min && temp < t_max) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr;
            get_sphere_uv(rec.normal, rec.u, rec.v);
            return true;
        }
        temp = (-b + discriminantSqrt) / a;
        if (temp > t_min && temp < t_max) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr;
            get_sphere_uv(rec.normal, rec.u, rec.v);
            return true;
        }
    }
    return false;
}

#endif /* SPHEREH */
