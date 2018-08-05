#ifndef HITABLE_HH
#define HITABLE_HH

#include "aabb.hh"
#include "ray.hh"

class material;

struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
    material *mat_ptr;
    float u;
    float v;
};

class hitable {
public:
    virtual bool hit(const ray& r,
                     float t_min,
                     float t_max,
                     hit_record& rec) const = 0;
    virtual bool bounding_box(float t0, float t1, aabb& box) const = 0;
};

class flip_normals : public hitable {
public:
    hitable *ptr;

    flip_normals(hitable *p) : ptr(p) {}

    virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const {
        if (ptr->hit(r, tmin, tmax, rec)) {
            rec.normal = -rec.normal;
            return true;
        } else {
            return false;
        }
    }
    virtual bool bounding_box(float t0, float t1, aabb& box) const {
        return ptr->bounding_box(t0, t1, box);
    }
};

#endif /* HITABLE_HH */
