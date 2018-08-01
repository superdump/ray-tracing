#include <iostream>
#include <limits>
#include <random>

#include "camera.hh"
#include "hitable_list.hh"
#include "ray.hh"
#include "sphere.hh"
#include "vec3.hh"

static const vec3 ones(1.0f, 1.0f, 1.0f);

std::random_device rd;
std::mt19937 rng(rd());
std::uniform_real_distribution<float> r01(0.0f, 1.0f);

vec3 random_in_unit_sphere() {
    vec3 p;
    do {
        p = 2.0f * vec3(r01(rng), r01(rng), r01(rng)) - ones;
    } while (p.squared_length() >= 1.0f);
    return p;
}

vec3 color(const ray& r, hitable *world) {
    hit_record rec;
    if (world->hit(r, 0.001f, std::numeric_limits<float>::max(), rec)) {
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        return 0.5f * color(ray(rec.p, target - rec.p), world);
    } else {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5f * (unit_direction.y() + 1.0f);
        return (1.0f - t) * ones + t * vec3(0.5f, 0.7f, 1.0f);
    }
}

int main() {
    int nx = 200;
    int ny = 100;
    int ns = 100;

    std::cout << "P3\n" << nx << " " << ny << "\n255\n";

    hitable *list[2];
    list[0] = new sphere(vec3(0.0f, 0.0f, -1.0f), 0.5f);
    list[1] = new sphere(vec3(0.0f, -100.5f, -1.0f), 100.0f);
    hitable *world = new hitable_list(list, 2);

    camera cam;

    for (int j = ny - 1; j >= 0; --j) {
        for (int i = 0; i < nx; ++i) {
            vec3 col(0.0f, 0.0f, 0.0f);

            for (int s = 0; s < ns; ++s) {
                float u = float(i + r01(rng)) / float(nx);
                float v = float(j + r01(rng)) / float(ny);
                ray r = cam.get_ray(u, v);
                vec3 p = r.point_at_parameter(2.0f);
                col += color(r, world);
            }
            col /= float(ns);
            col = vec3(sqrtf(col[0]), sqrtf(col[1]), sqrtf(col[2]));
            int ir = int(255.99f * col[0]);
            int ig = int(255.99f * col[1]);
            int ib = int(255.99f * col[2]);

            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
}
