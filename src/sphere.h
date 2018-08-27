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

    bool hit(const ray& r, float t_min, float t_max, hit_record& rec, uint32_t& state) const override;
    bool bounding_box(float t0, float t1, aabb &box) const override;
    float pdf_value(const vec3& o, const vec3& v, uint32_t& state) const override;
    vec3 random(const vec3& o, uint32_t& state) const override;

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

float sphere::pdf_value(const vec3& o, const vec3& v, uint32_t& state) const {
    hit_record rec;
    if (this->hit(ray(o, v), 0.001f, std::numeric_limits<float>::max(), rec, state)) {
        float cos_theta_max = sqrtf(1.0f - radius * radius / (center - o).squared_length());
        float solid_angle = 2.0f * M_PI * (1.0f - cos_theta_max);
        return 1.0f / solid_angle;
    } else {
        return 0.0f;
    }
}

vec3 sphere::random(const vec3& o, uint32_t& state) const {
    vec3 direction = center - o;
    float distance_squared = direction.squared_length();
    onb uvw;
    uvw.build_from_w(direction);
    return uvw.local(random_to_sphere(radius, distance_squared, state));
}

#endif /* SPHEREH */
