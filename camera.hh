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
    vec3 u, v, w;
    float lens_radius;

    // vfov is top to bottom in degrees
    camera(vec3 lookfrom, vec3 lookat, vec3 vup,
           float vfov, float aspect,
           float aperture, float focus_dist) {
        lens_radius = 0.5f * aperture;
        float theta = vfov * M_PI / 180.0f;
        float half_height = tanf(0.5f * theta);
        float half_width = aspect * half_height;
        origin = lookfrom;
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);
        // lower_left_corner = vec3(-half_width, -half_height, -1.0f);
        lower_left_corner = origin
            - half_width * focus_dist * u
            - half_height * focus_dist * v
            - focus_dist * w;
        horizontal = 2.0f * half_width * focus_dist * u;
        vertical = 2.0f * half_height * focus_dist * v;
    }

    ray get_ray(float s, float t) {
        vec3 rd = lens_radius * random_in_unit_disc();
        vec3 offset = u * rd.x() + v * rd.y();
        return ray(origin + offset,
                   lower_left_corner
                   + s * horizontal
                   + t * vertical
                   - origin
                   - offset);
    }
};

#endif /* CAMERA_HH */
