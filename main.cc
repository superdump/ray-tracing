#include <iostream>
#include <limits>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "aabb.hh"
#include "box.hh"
#include "bvh_node.hh"
#include "camera.hh"
#include "hitable_list.hh"
#include "material.hh"
#include "moving_sphere.hh"
#include "ray.hh"
#include "rng.hh"
#include "rotate.hh"
#include "sphere.hh"
#include "translate.hh"
#include "vec3.hh"
#include "volumes.hh"
#include "rect.hh"

vec3 color(const ray& r, hitable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001f, std::numeric_limits<float>::max(), rec)) {
        ray scattered;
        vec3 attenuation;
        vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return emitted + attenuation * color(scattered, world, depth + 1);
        } else {
            return emitted;
        }
    } else {
        return zeros;
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

hitable *earth_sphere() {
    int nx, ny, nn;
    unsigned char *tex_data = stbi_load("world.topo.bathy.200412.3x5400x2700.jpg", &nx, &ny, &nn, 0);
    material *mat = new lambertian(new image_texture(tex_data, nx, ny));
    int n = 50;
    hitable** list = new hitable*[n + 1];
    list[0] = new sphere(vec3(0.0f, 0.0f, 0.0f), 2.0f, mat);
    return new hitable_list(list, 1);
}

hitable *simple_light() {
    texture *pertext = new noise_texture(4.0f);
    hitable **list = new hitable*[4];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
    list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
    list[2] = new sphere(vec3(0, 6, 0), 1, new diffuse_light(new constant_texture(vec3(4,4,4))));
    list[3] = new xy_rect(3, 5, 1, 3, -2, new diffuse_light(new constant_texture(vec3(4,4,4))));
    return new hitable_list(list, 4);
}

hitable *cornell_box() {
    hitable **list = new hitable*[8];

    material *red = new lambertian(new constant_texture(vec3(0.65f, 0.05f, 0.05f)));
    material *white = new lambertian(new constant_texture(vec3(0.73f, 0.73f, 0.73f)));
    material *green = new lambertian(new constant_texture(vec3(0.12f, 0.45f, 0.15f)));
    material *light = new diffuse_light(new constant_texture(vec3(15.0f, 15.0f, 15.0f)));

    int i = 0;
    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
    list[i++] = new translate(new rotate_y(new box(vec3(0.0f, 0.0f, 0.0f), vec3(165.0f, 165.0f, 165.0f), white), -18.0f), vec3(130.0f, 0.0f, 65.0f));
    list[i++] = new translate(new rotate_y(new box(vec3(0.0f, 0.0f, 0.0f), vec3(165.0f, 330.0f, 165.0f), white), 15.0f), vec3(265.0f, 0.0f, 295.0f));

    return new hitable_list(list, i);
}

hitable *cornell_smoke() {
    hitable **list = new hitable*[8];

    material *red = new lambertian(new constant_texture(vec3(0.65f, 0.05f, 0.05f)));
    material *white = new lambertian(new constant_texture(vec3(0.73f, 0.73f, 0.73f)));
    material *green = new lambertian(new constant_texture(vec3(0.12f, 0.45f, 0.15f)));
    material *light = new diffuse_light(new constant_texture(vec3(7.0f, 7.0f, 7.0f)));

    int i = 0;
    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new xz_rect(113, 443, 127, 432, 554, light);
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
    hitable *b1 = new translate(new rotate_y(new box(vec3(0.0f, 0.0f, 0.0f), vec3(165.0f, 165.0f, 165.0f), white), -18.0f), vec3(130.0f, 0.0f, 65.0f));
    hitable *b2 = new translate(new rotate_y(new box(vec3(0.0f, 0.0f, 0.0f), vec3(165.0f, 330.0f, 165.0f), white), 15.0f), vec3(265.0f, 0.0f, 295.0f));
    list[i++] = new constant_medium(b1, 0.01f, new constant_texture(vec3(1.0f, 1.0f, 1.0f)));
    list[i++] = new constant_medium(b2, 0.01f, new constant_texture(vec3(0.0f, 0.0f, 0.0f)));

    return new hitable_list(list, i);
}

int main() {
    int nx = 800;
    int ny = 800;
    int ns = 1000;

    std::cout << "P3\n" << nx << " " << ny << "\n255\n";

    // hitable *world = random_scene();
    // hitable *world = two_spheres();
    // hitable *world = two_perlin_spheres();
    // hitable *world = earth_sphere();
    // hitable *world = simple_light();
    // hitable *world = cornell_box();
    hitable *world = cornell_smoke();

    vec3 lookfrom(278.0f, 278.0f, -800.0f);
    vec3 lookat(278.0f, 278.0f, 0.0f);
    float dist_to_focus = 10.0f;
    float aperture = 0.0f;
    float vfov = 40.0f;
    camera cam(lookfrom, lookat,
               vec3(0.0f, 1.0f, 0.0f),
               vfov, float(nx) / float(ny),
               aperture, dist_to_focus,
               0.0f, 1.0f);

    float progress = 0.0;
    time_t timer = time(NULL);
    int barWidth = 70;

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
        progress = double(ny - 1 - j) / double(ny);
        std::cerr << "[";
        int pos = barWidth * progress;
        for (int b = 0; b < barWidth; ++b) {
            if (b < pos)
                std::cerr << "=";
            else if (b == pos)
                std::cerr << ">";
            else
                std::cerr << " ";
        }
        std::cerr << "] " << int(progress * 100.0) << " %";

        double elapsed = difftime(time(NULL), timer);
        double remaining = (elapsed / progress) * (1.0 - progress);
        std::cerr << " e: " << int(elapsed) << "s - r: " << int(remaining) << "s - t: " << int(elapsed + remaining) << "s";
        std::cerr << "\r";
        std::cerr.flush();
    }
    std::cerr << std::endl;
}
