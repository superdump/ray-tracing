#ifndef CAMERA_HH
#define CAMERA_HH

#include <cmath>

#include "ray.hh"
#include "vec3.hh"

class camera {
public:
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;

    // vfov is top to bottom in degrees
    camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect) {
        vec3 u, v, w;
        float theta = vfov * M_PI / 180.0f;
        float half_height = tanf(0.5f * theta);
        float half_width = aspect * half_height;
        origin = lookfrom;
        w = unit_vector(origin - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);
        // lower_left_corner = vec3(-half_width, -half_height, -1.0f);
        lower_left_corner = origin - half_width * u - half_height * v - w;
        horizontal = 2.0f * half_width * u;
        vertical = 2.0f * half_height * v;
    }

    ray get_ray(float s, float t) {
        return ray(origin, lower_left_corner + s * horizontal + t * vertical - origin);
    }
};

#endif /* CAMERA_HH */
