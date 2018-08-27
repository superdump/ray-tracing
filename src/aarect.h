#ifndef AARECTH
#define AARECTH

#include "aabb.h"
#include "hitable.h"
#include "ray.h"
#include "vec3.h"

class xy_rect : public hitable {
public:
    xy_rect() {}
    xy_rect(float _x0, float _x1, float _y0, float _y1, float _k, material *mat)
        : mat_ptr(mat), x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k) {}

    bool hit(const ray& r, float t0, float t1, hit_record& rec, uint32_t& state) const override;
    bool bounding_box(float t0, float t1, aabb& box) const override {
        box = aabb(vec3(x0, y0, k - 0.0001f), vec3(x1, y1, k + 0.0001f));
        return true;
    }
    float pdf_value(const vec3& o, const vec3& v, uint32_t& state) const override {
        hit_record rec;
        if (this->hit(ray(o, v), 0.001f, std::numeric_limits<float>::max(), rec, state)) {
            float area = (x1 - x0) * (y1 - y0);
            float v_squared_length = v.squared_length();
            float distance_squared = rec.t * rec.t * v_squared_length;
            float cosine = fabs(dot(v, rec.normal) / sqrtf(v_squared_length));
            return distance_squared / (cosine * area);
        } else {
            return 0.0f;
        }
    }
    vec3 random(const vec3& o, uint32_t& state) const override {
        vec3 random_point = vec3(
            x0 + RandomFloat01(state) * (x1 - x0),
            y0 + RandomFloat01(state) * (y1 - y0),
            k
        );
        return random_point - o;
    }

    material *mat_ptr;
    float x0, x1, y0, y1, k;
};

bool xy_rect::hit(const ray &r, float t0, float t1, hit_record &rec, uint32_t& state) const {
    float t = (k - r.origin().z()) / r.direction().z();
    if (t < t0 || t > t1) {
        return false;
    }
    float x = r.origin().x() + t * r.direction().x();
    float y = r.origin().y() + t * r.direction().y();
    if (x < x0 || x > x1 || y < y0 || y > y1) {
        return false;
    }
    rec.u = (x - x0) / (x1 - x0);
    rec.v = (y - y0) / (y1 - y0);
    rec.t = t;
    rec.mat_ptr = mat_ptr;
    rec.p = r.point_at_parameter(t);
    rec.normal = vec3(0.0f, 0.0f, 1.0f);
    return true;
}

class xz_rect : public hitable {
public:
    xz_rect() {}
    xz_rect(float _x0, float _x1, float _z0, float _z1, float _k, material *mat)
        : mat_ptr(mat), x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k) {}

    bool hit(const ray& r, float t0, float t1, hit_record& rec, uint32_t& state) const override;
    bool bounding_box(float t0, float t1, aabb& box) const override {
        box = aabb(vec3(x0, k - 0.0001f, z0), vec3(x1, k + 0.0001f, z1));
        return true;
    }
    float pdf_value(const vec3& o, const vec3& v, uint32_t& state) const override {
        hit_record rec;
        if (this->hit(ray(o, v), 0.001f, std::numeric_limits<float>::max(), rec, state)) {
            float area = (x1 - x0) * (z1 - z0);
            float v_squared_length = v.squared_length();
            float distance_squared = rec.t * rec.t * v_squared_length;
            float cosine = fabs(dot(v, rec.normal) / sqrtf(v_squared_length));
            return distance_squared / (cosine * area);
        } else {
            return 0.0f;
        }
    }
    vec3 random(const vec3& o, uint32_t& state) const override {
        vec3 random_point = vec3(
            x0 + RandomFloat01(state) * (x1 - x0),
            k,
            z0 + RandomFloat01(state) * (z1 - z0)
        );
        return random_point - o;
    }

    material *mat_ptr;
    float x0, x1, z0, z1, k;
};

bool xz_rect::hit(const ray &r, float t0, float t1, hit_record &rec, uint32_t& state) const {
    float t = (k - r.origin().y()) / r.direction().y();
    if (t < t0 || t > t1) {
        return false;
    }
    float x = r.origin().x() + t * r.direction().x();
    float z = r.origin().z() + t * r.direction().z();
    if (x < x0 || x > x1 || z < z0 || z > z1) {
        return false;
    }
    rec.u = (x - x0) / (x1 - x0);
    rec.v = (z - z0) / (z1 - z0);
    rec.t = t;
    rec.mat_ptr = mat_ptr;
    rec.p = r.point_at_parameter(t);
    rec.normal = vec3(0.0f, 1.0f, 0.0f);
    return true;
}

class yz_rect : public hitable {
public:
    yz_rect() {}
    yz_rect(float _y0, float _y1, float _z0, float _z1, float _k, material *mat)
        : mat_ptr(mat), y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k) {}

    bool hit(const ray& r, float t0, float t1, hit_record& rec, uint32_t& state) const override;
    bool bounding_box(float t0, float t1, aabb& box) const override {
        box = aabb(vec3(k - 0.0001f, y0, z0), vec3(k + 0.0001f, y1, z1));
        return true;
    }
    float pdf_value(const vec3& o, const vec3& v, uint32_t& state) const override {
        hit_record rec;
        if (this->hit(ray(o, v), 0.001f, std::numeric_limits<float>::max(), rec, state)) {
            float area = (y1 - y0) * (z1 - z0);
            float v_squared_length = v.squared_length();
            float distance_squared = rec.t * rec.t * v_squared_length;
            float cosine = fabs(dot(v, rec.normal) / sqrtf(v_squared_length));
            return distance_squared / (cosine * area);
        } else {
            return 0.0f;
        }
    }
    vec3 random(const vec3& o, uint32_t& state) const override {
        vec3 random_point = vec3(
            k,
            y0 + RandomFloat01(state) * (y1 - y0),
            z0 + RandomFloat01(state) * (z1 - z0)
        );
        return random_point - o;
    }

    material *mat_ptr;
    float y0, y1, z0, z1, k;
};

bool yz_rect::hit(const ray &r, float t0, float t1, hit_record &rec, uint32_t& state) const {
    float t = (k - r.origin().x()) / r.direction().x();
    if (t < t0 || t > t1) {
        return false;
    }
    float y = r.origin().y() + t * r.direction().y();
    float z = r.origin().z() + t * r.direction().z();
    if (y < y0 || y > y1 || z < z0 || z > z1) {
        return false;
    }
    rec.u = (y - y0) / (y1 - y0);
    rec.v = (z - z0) / (z1 - z0);
    rec.t = t;
    rec.mat_ptr = mat_ptr;
    rec.p = r.point_at_parameter(t);
    rec.normal = vec3(1.0f, 0.0f, 0.0f);
    return true;
}

#endif /* AARECTH */
