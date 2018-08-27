#ifndef MOVINGSPHEREH
#define MOVINGSPHEREH

#include "hitable.h"
#include "material.h"
#include "ray.h"
#include "sphere.h"
#include "vec3.h"

class moving_sphere : public hitable {
public:
    moving_sphere() {}
    moving_sphere(vec3 cen0, vec3 cen1, float t0, float t1,
                  float r, material *m)
                    : center0(cen0), center1(cen1),
                      time0(t0), time1(t1),
                      radius(r), mat_ptr(m) {}

    bool hit(const ray& r, float t_min, float t_max, hit_record& rec, uint32_t& state) const override;
    bool bounding_box(float t0, float t1, aabb &box) const override;
    float pdf_value(const vec3& o, const vec3& v, uint32_t& state) const override;
    vec3 random(const vec3& o, uint32_t& state) const override;
    vec3 center(float t) const;

    vec3 center0, center1;
    float time0, time1;
    float radius;
    material *mat_ptr;
};

vec3 moving_sphere::center(float t) const {
    return center0 + ((t - time0) / (time1 - time0)) * (center1 - center0);
}

bool moving_sphere::bounding_box(float t0, float t1, aabb &box) const {
    vec3 half_diagonal(radius, radius, radius);
    vec3 c0 = center(t0);
    vec3 c1 = center(t1);
    aabb box0(c0 - half_diagonal, c0 + half_diagonal);
    aabb box1(c1 - half_diagonal, c1 + half_diagonal);
    box = surrounding_box(box0, box1);
    return true;
}

bool moving_sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec, uint32_t& state) const {
    vec3 oc = r.origin() - center(r.time());
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
            rec.normal = (rec.p - center(r.time())) / radius;
            rec.mat_ptr = mat_ptr;
            get_sphere_uv(rec.normal, rec.u, rec.v);
            return true;
        }
        temp = (-b + discriminantSqrt) / a;
        if (temp > t_min && temp < t_max) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center(r.time())) / radius;
            rec.mat_ptr = mat_ptr;
            get_sphere_uv(rec.normal, rec.u, rec.v);
            return true;
        }
    }
    return false;
}

float moving_sphere::pdf_value(const vec3& o, const vec3& v, uint32_t& state) const {
    hit_record rec;
    if (this->hit(ray(o, v), 0.001f, std::numeric_limits<float>::max(), rec, state)) {
        float cos_theta_max = sqrtf(1.0f - radius * radius / (center(0.0f) - o).squared_length());
        float solid_angle = 2.0f * M_PI * (1.0f - cos_theta_max);
        return 1.0f / solid_angle;
    } else {
        return 0.0f;
    }
}

vec3 moving_sphere::random(const vec3& o, uint32_t& state) const {
    vec3 direction = center(0.0f) - o;
    float distance_squared = direction.squared_length();
    onb uvw;
    uvw.build_from_w(direction);
    return uvw.local(random_to_sphere(radius, distance_squared, state));
}

#endif /* MOVINGSPHEREH */
