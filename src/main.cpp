#include <atomic>
#include <chrono>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <thread>

using namespace std::chrono_literals;

#include "docopt.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "MiniFB.h"

#include "aabb.h"
#include "aarect.h"
#include "box.h"
#include "bvh.h"
#include "camera.h"
#include "constant_medium.h"
#include "hitable_list.h"
#include "material.h"
#include "moving_sphere.h"
#include "ray.h"
#include "rng.h"
#include "sphere.h"
#include "surface_texture.h"
#include "vec3.h"

#include "TaskScheduler.h"

enki::TaskScheduler g_TS;

hitable *final(std::string tex) {
    int nb = 20;
    hitable **list = new hitable *[30];
    hitable **boxlist = new hitable *[10000];
    hitable **boxlist2 = new hitable *[10000];
    material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
    material *ground = new lambertian(new constant_texture(vec3(0.48, 0.83, 0.53)));
    int b = 0;
    for (int i = 0; i < nb; i++) {
        for (int j = 0; j < nb; j++) {
            float w = 100;
            float x0 = -1000 + i * w;
            float z0 = -1000 + j * w;
            float y0 = 0;
            float x1 = x0 + w;
            float y1 = 100 * (r01(rng) + 0.01);
            float z1 = z0 + w;
            boxlist[b++] = new box(vec3(x0, y0, z0), vec3(x1, y1, z1), ground);
        }
    }
    int l = 0;
    list[l++] = new bvh_node(boxlist, b, 0, 1);
    material *light = new diffuse_light(new constant_texture(vec3(7, 7, 7)));
    list[l++] = new xz_rect(123, 423, 147, 412, 554, light);
    vec3 center(400, 400, 200);
    list[l++] = new moving_sphere(center, center + vec3(30, 0, 0), 0, 1, 50, new lambertian(new constant_texture(vec3(0.7, 0.3, 0.1))));
    list[l++] = new sphere(vec3(260, 150, 45), 50, new dielectric(1.5));
    list[l++] = new sphere(vec3(0, 150, 145), 50, new metal(vec3(0.8, 0.8, 0.9), 10.0));
    hitable *boundary = new sphere(vec3(360, 150, 145), 70, new dielectric(1.5));
    list[l++] = boundary;
    list[l++] = new constant_medium(boundary, 0.2, new constant_texture(vec3(0.2, 0.4, 0.9)));
    boundary = new sphere(vec3(0, 0, 0), 5000, new dielectric(1.5));
    list[l++] = new constant_medium(boundary, 0.0001, new constant_texture(vec3(1.0, 1.0, 1.0)));
    int nx, ny, nn;
    unsigned char *tex_data = stbi_load(tex.c_str(), &nx, &ny, &nn, 0);
    material *emat = new lambertian(new image_texture(tex_data, nx, ny));
    list[l++] = new sphere(vec3(400, 200, 400), 100, emat);
    texture *pertext = new noise_texture(0.1);
    list[l++] = new sphere(vec3(220, 280, 300), 80, new lambertian(pertext));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxlist2[j] = new sphere(vec3(165 * r01(rng), 165 * r01(rng), 165 * r01(rng)), 10, white);
    }
    list[l++] = new translate(new rotate_y(new bvh_node(boxlist2, ns, 0.0, 1.0), 15), vec3(-100, 270, 395));
    return new hitable_list(list, l);
}

hitable *cornell_final(std::string tex) {
    hitable **list = new hitable*[30];
    // hitable **boxlist = new hitable*[10000];
    // texture *pertext = new noise_texture(0.1);
    // int nx, ny, nn;
    // unsigned char *tex_data = stbi_load(tex.c_str(), &nx, &ny, &nn, 0);
    // material *mat =  new lambertian(new image_texture(tex_data, nx, ny));
    int i = 0;
    material *red = new lambertian( new constant_texture(vec3(0.65, 0.05, 0.05)) );
    material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)) );
    material *green = new lambertian( new constant_texture(vec3(0.12, 0.45, 0.15)) );
    material *light = new diffuse_light( new constant_texture(vec3(7, 7, 7)) );
    //list[i++] = new sphere(vec3(260, 50, 145), 50,mat);
    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new xz_rect(123, 423, 147, 412, 554, light);
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
    /*
    hitable *boundary = new sphere(vec3(160, 50, 345), 50, new dielectric(1.5));
    list[i++] = boundary;
    list[i++] = new constant_medium(boundary, 0.2, new constant_texture(vec3(0.2, 0.4, 0.9)));
    list[i++] = new sphere(vec3(460, 50, 105), 50, new dielectric(1.5));
    list[i++] = new sphere(vec3(120, 50, 205), 50, new lambertian(pertext));
    int ns = 10000;
    for (int j = 0; j < ns; j++) {
        boxlist[j] = new sphere(vec3(165*drand48(), 330*drand48(), 165*drand48()), 10, white);
    }
    list[i++] =   new translate(new rotate_y(new bvh_node(boxlist,ns, 0.0, 1.0), 15), vec3(265,0,295));
    */
    hitable *boundary2 = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), new dielectric(1.5)), -18), vec3(130,0,65));
    list[i++] = boundary2;
    list[i++] = new constant_medium(boundary2, 0.2, new constant_texture(vec3(0.9, 0.9, 0.9)));
    return new hitable_list(list,i);
}

hitable *cornell_balls() {
    hitable **list = new hitable*[9];
    int i = 0;
    material *red = new lambertian( new constant_texture(vec3(0.65, 0.05, 0.05)) );
    material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)) );
    material *green = new lambertian( new constant_texture(vec3(0.12, 0.45, 0.15)) );
    material *light = new diffuse_light( new constant_texture(vec3(5, 5, 5)) );
    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new xz_rect(113, 443, 127, 432, 554, light);
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
    hitable *boundary = new sphere(vec3(160, 100, 145), 100, new dielectric(1.5));
    list[i++] = boundary;
    list[i++] = new constant_medium(boundary, 0.1, new constant_texture(vec3(1.0, 1.0, 1.0)));
    list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white),  15), vec3(265,0,295));
    return new hitable_list(list,i);
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
    for (int a = -10; a < 10; ++a) {
        for (int b = -10; b < 10; ++b) {
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
    list[i++] = new sphere(vec3(0.0f, 0.0f, 0.0f), 2000.0f, new diffuse_light(new constant_texture(vec3(0.5f, 0.7f, 1.0f))));

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
    list[2] = new sphere(vec3(0.0f, 0.0f, 0.0f), 2000.0f, new diffuse_light(new constant_texture(vec3(0.5f, 0.7f, 1.0f))));
    return new hitable_list(list, 3);
}

hitable *two_perlin_spheres() {
    texture *noise = new noise_texture(5.0f);
    int n = 50;
    hitable** list = new hitable*[n + 1];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(noise));
    list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(noise));
    list[2] = new sphere(vec3(0.0f, 0.0f, 0.0f), 2000.0f, new diffuse_light(new constant_texture(vec3(0.5f, 0.7f, 1.0f))));
    return new hitable_list(list, 3);
}

hitable *earth(std::string tex) {
    int nx, ny, nn;
    unsigned char *tex_data = stbi_load(tex.c_str(), &nx, &ny, &nn, 0);
    material *mat = new lambertian(new image_texture(tex_data, nx, ny));
    int n = 50;
    hitable** list = new hitable*[n + 1];
    list[0] = new sphere(vec3(0.0f, 0.0f, 0.0f), 2.0f, mat);
    list[1] = new sphere(vec3(0.0f, 0.0f, 0.0f), 2000.0f, new diffuse_light(new constant_texture(vec3(0.5f, 0.7f, 1.0f))));
    return new hitable_list(list, 2);
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

struct scene {
    camera cam;
    hitable *world;
};

scene get_scene(std::string s, std::string texture, float aspect) {
    vec3 lookfrom(478.0f, 278.0f, -600.0f);
    vec3 lookat(278.0f, 278.0f, 0.0f);
    float dist_to_focus = 10.0f;
    float aperture = 0.0f;
    float vfov = 40.0f;
    camera cam(lookfrom, lookat,
               vec3(0.0f, 1.0f, 0.0f),
               vfov, aspect,
               aperture, dist_to_focus,
               0.0f, 1.0f);

    if (s == "random_scene") {
        vec3 lookfrom(13.0f, 2.0f, 3.0f);
        vec3 lookat(0.0f, 0.0f, 0.0f);
        vfov = 20.0f;
        return scene {
            cam: camera(lookfrom, lookat,
                vec3(0.0f, 1.0f, 0.0f),
                vfov, aspect,
                aperture, dist_to_focus,
                0.0f, 1.0f),
            world: random_scene()
        };
    } else if (s == "two_spheres") {
        vec3 lookfrom(13.0f, 2.0f, 3.0f);
        vec3 lookat(0.0f, 0.0f, 0.0f);
        vfov = 20.0f;
        return scene {
            cam: camera(lookfrom, lookat,
                vec3(0.0f, 1.0f, 0.0f),
                vfov, aspect,
                aperture, dist_to_focus,
                0.0f, 1.0f),
            world: two_spheres()
        };
    } else if (s == "two_perlin_spheres") {
        vec3 lookfrom(13.0f, 2.0f, 3.0f);
        vec3 lookat(0.0f, 0.0f, 0.0f);
        vfov = 20.0f;
        return scene {
            cam: camera(lookfrom, lookat,
                vec3(0.0f, 1.0f, 0.0f),
                vfov, aspect,
                aperture, dist_to_focus,
                0.0f, 1.0f),
            world: two_perlin_spheres()
        };
    } else if (s == "earth") {
        vec3 lookfrom(0.0f, 0.0f, 15.0f);
        vec3 lookat(0.0f, 0.0f, 0.0f);
        vfov = 20.0f;
        return scene {
            cam: camera(lookfrom, lookat,
                vec3(0.0f, 1.0f, 0.0f),
                vfov, aspect,
                aperture, dist_to_focus,
                0.0f, 1.0f),
            world: earth(texture)
        };
    } else if (s == "simple_light") {
        vec3 lookfrom(26.0f, 4.0f, 6.0f);
        vec3 lookat(0.0f, 0.0f, 0.0f);
        vfov = 20.0f;
        return scene {
            cam: camera(lookfrom, lookat,
                vec3(0.0f, 1.0f, 0.0f),
                vfov, aspect,
                aperture, dist_to_focus,
                0.0f, 1.0f),
            world: simple_light()
        };
    } else if (s == "cornell_box") {
        vec3 lookfrom(278.0f, 278.0f, -800.0f);
        vec3 lookat(278.0f, 278.0f, 0.0f);
        return scene {
            cam: camera(lookfrom, lookat,
                vec3(0.0f, 1.0f, 0.0f),
                vfov, aspect,
                aperture, dist_to_focus,
                0.0f, 1.0f),
            world: cornell_box()
        };
    } else if (s == "cornell_smoke") {
        vec3 lookfrom(278.0f, 278.0f, -800.0f);
        vec3 lookat(278.0f, 278.0f, 0.0f);
        return scene {
            cam: camera(lookfrom, lookat,
                vec3(0.0f, 1.0f, 0.0f),
                vfov, aspect,
                aperture, dist_to_focus,
                0.0f, 1.0f),
            world: cornell_smoke()
        };
    } else if (s == "cornell_balls") {
        vec3 lookfrom(278.0f, 278.0f, -800.0f);
        vec3 lookat(278.0f, 278.0f, 0.0f);
        return scene {
            cam: camera(lookfrom, lookat,
                vec3(0.0f, 1.0f, 0.0f),
                vfov, aspect,
                aperture, dist_to_focus,
                0.0f, 1.0f),
            world: cornell_balls()
        };
    } else if (s == "cornell_final") {
        vec3 lookfrom(278.0f, 278.0f, -800.0f);
        vec3 lookat(278.0f, 278.0f, 0.0f);
        return scene {
            cam: camera(lookfrom, lookat,
                vec3(0.0f, 1.0f, 0.0f),
                vfov, aspect,
                aperture, dist_to_focus,
                0.0f, 1.0f),
            world: cornell_final(texture)
        };
    } else if (s == "final") {
        vec3 lookfrom(478.0f, 278.0f, -600.0f);
        vec3 lookat(278.0f, 278.0f, 0.0f);
        return scene {
            cam: camera(lookfrom, lookat,
                vec3(0.0f, 1.0f, 0.0f),
                vfov, aspect,
                aperture, dist_to_focus,
                0.0f, 1.0f),
            world: final(texture)
        };
    }
}

vec3 color(const ray& r, const hitable *world, int nb, uint64_t& ray_count, int depth, uint32_t& state) {
    ++ray_count;
    hit_record rec;
    if (world->hit(r, 0.001f, std::numeric_limits<float>::max(), rec, state)) {
        ray scattered;
        vec3 albedo;
        vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
        float pdf;
        if (depth < nb && rec.mat_ptr->scatter(r, rec, albedo, scattered, pdf, state)) {
            return emitted
                + albedo
                    * rec.mat_ptr->scattering_pdf(r, rec, scattered)
                    * color(scattered, world, nb, ray_count, depth + 1, state)
                    / pdf;
        } else {
            return emitted;
        }
    } else {
        return zeros;
    }
}

struct ParallelTaskSet : enki::ITaskSet {
    ParallelTaskSet(int nx, int ny, int ns, int nb, camera& c, hitable *w, uint8_t *i, unsigned int *fb_)
            : nx(nx), ny(ny), ns(ns), nb(nb), cam(c), world(w), image(i), fb(fb_) {
        m_SetSize = ny;
        ray_count = 0;
        lines_complete = 0;
    }
    virtual void ExecuteRange(enki::TaskSetPartition range, uint32_t threadnum) {
        uint64_t local_ray_count = 0;
        for (uint32_t j = range.start; j < range.end; ++j) {
            uint32_t state = (j * 9781) | 1;
            for (int i = 0; i < nx; ++i) {
                vec3 col(0.0f, 0.0f, 0.0f);

                for (int s = 0; s < ns; ++s) {
                    float u = float(i + RandomFloat01(state)) / float(nx);
                    float v = float(j + RandomFloat01(state)) / float(ny);
                    ray r = cam.get_ray(u, v, state);
                     // vec3 p = r.point_at_parameter(2.0f);
                    col += color(r, world, nb, local_ray_count, 0, state);
                }
                col /= float(ns);
                // gamma correction
                col = vec3(sqrtf(col[0]), sqrtf(col[1]), sqrtf(col[2]));
                // [0.0, 1.0] -> [0, 255]
                int ir = int(255.99f * col[0]);
                int ig = int(255.99f * col[1]);
                int ib = int(255.99f * col[2]);
                // clipping
                ir = ir > 255 ? 255 : ir;
                ig = ig > 255 ? 255 : ig;
                ib = ib > 255 ? 255 : ib;

                int index = (ny - 1 - j) * nx + i;
                int index3 = 3 * index;
                image[index3 + 0] = ir;
                image[index3 + 1] = ig;
                image[index3 + 2] = ib;
                fb[index] = MFB_RGB(ir, ig, ib);
            }
            ++lines_complete;
            ray_count += local_ray_count;
            local_ray_count = 0;
            float progress = double(lines_complete) / double(ny);
            std::stringstream stream;
            stream << "[";
            int pos = BARWIDTH * progress;
            for (int b = 0; b < BARWIDTH; ++b) {
                if (b < pos)
                    stream << "=";
                else if (b == pos)
                    stream << ">";
                else
                    stream << " ";
            }
            stream << "] " << int(progress * 100.0) << " %";

            double elapsed = difftime(time(NULL), TIMER);
            double remaining = (elapsed / progress) * (1.0 - progress);
            int ela_mins = elapsed / 60.0;
            int ela_secs = elapsed - (ela_mins * 60.0);
            int rem_mins = remaining / 60.0;
            int rem_secs = remaining - (rem_mins * 60.0);
            double total = elapsed + remaining;
            int tot_mins = total / 60.0;
            int tot_secs = total - (tot_mins * 60.0);
            if (ela_mins >= 0 && ela_secs >= 0 && rem_mins >= 0 && rem_secs >= 0 && tot_mins >= 0 && tot_secs >= 0) {
                double mrays_per_sec = double(ray_count) / (1000000.0 * elapsed);
                stream << " Mrays/s: " << mrays_per_sec
                    << " - e: " << ela_mins << "m" << ela_secs
                    << "s - r: " << rem_mins << "m" << rem_secs
                    << "s - t: " << tot_mins << "m" << tot_secs << "s";
            }
            stream << "\r";
            std::cerr << stream.str();
            std::cerr.flush();
        }
    }
    const int nx, ny, ns, nb;
    const camera& cam;
    const hitable *world;
    uint8_t *image;
    unsigned int *fb;
    std::atomic<uint64_t> ray_count;
    std::atomic<int> lines_complete;
    const time_t TIMER = time(NULL);
    const int BARWIDTH = 70;
};

static const char USAGE[] =
R"(Ray Tracing in One Week.

    Usage:
      rtiow [-s SCENE] [-w WIDTH] [-h HEIGHT] [-r RAYS_PER_PIXEL]
            [-b MAX_BOUNCES_PER_RAY] [-t TEXTURE] [-o OUTPUT]
      rtiow (-h | --help)
      rtiow --version

    Options:
      --help                Show this screen.
      --version             Show version.
      -w, --width=WIDTH     Width of output image. [default: 800]
      -h, --height=HEIGHT   Height of output image. [default: 800]
      -s, --scene=SCENE     Name of scene to be rendered. One of:
                            earth, two_spheres, final, cornell_final,
                            cornell_balls cornell_smoke cornell_box,
                            two_perlin_spheres simple_light random_scene
                            [default: final]
      -o, --output=OUTPUT   Output PNG file path. [default: image.png]
      -r, --rays-per-pixel=RAYS_PER_PIXEL Number of rays to cast per pixel.
                            [default: 100]
      -b, --bounces-per-ray=MAX_BOUNCES_PER_RAY Maximum number of bounces per
                            ray. [default: 50]
      -t, --texture=TEXTURE Relative path to a JPEG texture.
                            [default: ../data/world.topo.bathy.200412.3x5400x2700.jpg]
)";

int main(int argc, const char** argv)
{
    std::map<std::string, docopt::value> args = docopt::docopt(
        USAGE, { argv + 1, argv + argc }, true, "Ray Tracing in One Week");

    int nx = args["--width"].asLong();
    int ny = args["--height"].asLong();
    int ns = args["--rays-per-pixel"].asLong();
    int nb = args["--bounces-per-ray"].asLong();

    uint8_t *image = new uint8_t[nx * ny * 3];
    unsigned int *fb = new unsigned int[nx * ny];

    if (!mfb_open("Ray Tracing In One Weekend", nx, ny)) {
        std::cerr << "ERROR: Failed to open minifb window.\n";
    }

    auto selected_scene = args["--scene"].asString();
    auto texture = args["--texture"].asString();
    scene scene = get_scene(selected_scene, texture, float(nx) / float(ny));

    std::cerr << "Rendering '" << selected_scene << "' scene at " << nx << "x" << ny
        << " with " << ns << " rays/px and up to " << nb << " bounces\n";
    std::cerr << "Using " << enki::GetNumHardwareThreads() << " threads\n";
    g_TS.Initialize();
    ParallelTaskSet task(nx, ny, ns, nb, scene.cam, scene.world, image, fb);
    g_TS.AddTaskSetToPipe(&task);
    while (!task.GetIsComplete() && mfb_update(fb) != -1) {
        std::this_thread::sleep_for(100ms);
    }
    std::cerr << std::endl;

    auto output = args["--output"].asString();
    std::cerr << "Writing PNG to: " << output << "\n";
    stbi_write_png(output.c_str(), nx, ny, 3, image, nx * 3);
}
