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

    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
    virtual bool bounding_box(float t0, float t1, aabb &box) const;
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

bool moving_sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
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

#endif /* MOVINGSPHEREH */
