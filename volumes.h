#ifndef FOGH
#define FOGH

#include "aabb.h"
#include "hitable.h"
#include "material.h"
#include "ray.h"
#include "texture.h"

class constant_medium : public hitable {
public:
    hitable *boundary;
    float density;
    material *phase_function;

    constant_medium(hitable *b, float d, texture *a) : boundary(b), density(d) {
        phase_function = new isotropic(a);
    }

    virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
    virtual bool bounding_box(float t0, float t1, aabb& box) const {
        return boundary->bounding_box(t0, t1, box);
    }
};

bool constant_medium::hit(const ray &r, float tmin, float tmax, hit_record &rec) const {
    // bool db = r01(rng) < 0.00001f;
    bool db = false;
    hit_record rec1, rec2;
    if (boundary->hit(r, -std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), rec1)) {
        if (boundary->hit(r, rec1.t + 0.0001f, std::numeric_limits<float>::max(), rec2)) {
            if (db) {
                std::cerr << "\nt0 t0 " << rec1.t << " " << rec2.t << "\n";
            }
            if (rec1.t < tmin) {
                rec1.t = tmin;
            }
            if (rec2.t > tmax) {
                rec2.t = tmax;
            }
            if (rec1.t >= rec2.t) {
                return false;
            }
            if (rec1.t < 0.0f) {
                rec1.t = 0.0f;
            }
            float dir_length = r.direction().length();
            float distance_inside_boundary = (rec2.t - rec1.t) * dir_length;
            float hit_distance = -(1.0f / density) * logf(r01(rng));
            if (hit_distance < distance_inside_boundary) {
                if (db) {
                    std::cerr << "hit_distance = " << hit_distance << "\n";
                }
                rec.t += hit_distance / dir_length;
                if (db) {
                    std::cerr << "rec.t = " << rec.t << "\n";
                }
                rec.p = r.point_at_parameter(rec.t);
                if (db) {
                    std::cerr << "rec.p = " << rec.p << "\n";
                }
                rec.normal = vec3(1.0f, 0.0f, 0.0f); // arbitrary
                rec.mat_ptr = phase_function;
                return true;
            }
        }
    }
    return false;
}

#endif /* FOGH */
