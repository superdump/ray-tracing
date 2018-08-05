#include <iostream>
#include <limits>

#include "aabb.hh"
#include "bvh_node.hh"
#include "camera.hh"
#include "hitable_list.hh"
#include "material.hh"
#include "moving_sphere.hh"
#include "ray.hh"
#include "rng.hh"
#include "sphere.hh"
#include "vec3.hh"

vec3 color(const ray& r, hitable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001f, std::numeric_limits<float>::max(), rec)) {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return attenuation * color(scattered, world, depth + 1);
        } else {
            return zeros;
        }
    } else {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5f * (unit_direction.y() + 1.0f);
        return (1.0f - t) * ones + t * vec3(0.5f, 0.7f, 1.0f);
    }
}

hitable *random_scene() {
    int n = 500;
    hitable **list = new hitable*[n + 1];
    texture *checker = new checker_texture(
        new constant_texture(vec3(0.2f, 0.3f, 0.1f)),
        new constant_texture(vec3(0.9f, 0.9f, 0.9f))
    );
    list[0] = new sphere(vec3(0.0f, -1000.0f, 0.0f), 1000.0f, new lambertian(checker));
    int i = 1;
    for (int a = -11; a < 11; ++a) {
        for (int b = -11; b < 11; ++b) {
            float choose_mat = r01(rng);
            vec3 center(a + 0.9f * r01(rng), 0.2f, b + 0.9f * r01(rng));
            if ((center - vec3(4.0f, 0.2f, 0.0f)).length() > 0.9f) {
                if (choose_mat < 0.8f) { // diffuse
                    list[i++] = new moving_sphere(center, center + vec3(0.0f, 0.5f * r01(rng), 0.0f), 0.0, 1.0, 0.2f,
                        new lambertian(new constant_texture(vec3(r01(rng) * r01(rng),
                                            r01(rng) * r01(rng),
                                            r01(rng) * r01(rng)))));
                } else if (choose_mat < 0.95f) { // metal
                    list[i++] = new sphere(center, 0.2f,
                        new metal(vec3(0.5f * (1.0f + r01(rng)),
                                       0.5f * (1.0f + r01(rng)),
                                       0.5f * (1.0f + r01(rng))),
                                  0.5f * r01(rng)));
                } else { // glass
                    list[i++] = new sphere(center, 0.2f,
                        new dielectric(1.5f));
                }
            }
        }
    }
    list[i++] = new sphere(vec3(0.0f, 1.0f, 0.0f), 1.0f, new dielectric(1.5f));
    list[i++] = new sphere(vec3(-4.0f, 1.0f, 0.0f), 1.0f, new lambertian(new constant_texture(vec3(0.4f, 0.2f, 0.1f))));
    list[i++] = new sphere(vec3(4.0f, 1.0f, 0.0f), 1.0f, new metal(vec3(0.7f, 0.6f, 0.5f), 0.0f));

    // return new hitable_list(list, i);
    return new bvh_node(list, i, 0.0, 1.0);
}

hitable *two_spheres() {
    texture *checker = new checker_texture(
        new constant_texture(vec3(0.2f, 0.3f, 0.1f)),
        new constant_texture(vec3(0.9f, 0.9f, 0.9f))
    );
    int n = 50;
    hitable** list = new hitable*[n + 1];
    list[0] = new sphere(vec3(0, -10, 0), 10, new lambertian(checker));
    list[1] = new sphere(vec3(0, 10, 0), 10, new lambertian(checker));
    return new hitable_list(list, 2);
}

hitable *two_perlin_spheres() {
    texture *noise = new noise_texture(5.0f);
    int n = 50;
    hitable** list = new hitable*[n + 1];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(noise));
    list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(noise));
    return new hitable_list(list, 2);
}

int main() {
    int nx = 1280;
    int ny = 720;
    int ns = 25;

    std::cout << "P3\n" << nx << " " << ny << "\n255\n";

    // hitable *world = random_scene();
    // hitable *world = two_spheres();
    hitable *world = two_perlin_spheres();

    vec3 lookfrom(13.0f, 2.0f, 3.0f);
    vec3 lookat(0.0f, 0.0f, 0.0f);
    float dist_to_focus = 10.0f;
    float aperture = 0.0f;
    camera cam(lookfrom, lookat,
               vec3(0.0f, 1.0f, 0.0f),
               20.0f, float(nx) / float(ny),
               aperture, dist_to_focus,
               0.0f, 1.0f);

    for (int j = ny - 1; j >= 0; --j) {
        for (int i = 0; i < nx; ++i) {
            vec3 col(0.0f, 0.0f, 0.0f);

            for (int s = 0; s < ns; ++s) {
                float u = float(i + r01(rng)) / float(nx);
                float v = float(j + r01(rng)) / float(ny);
                ray r = cam.get_ray(u, v);
                // vec3 p = r.point_at_parameter(2.0f);
                col += color(r, world, 0);
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
