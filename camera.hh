#ifndef CAMERA_HH
#define CAMERA_HH

#include "ray.hh"

class camera {
public:
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;

    camera() {
        lower_left_corner = vec3(-2.0f, -1.0f, -1.0f);
        horizontal = vec3(4.0f, 0.0f, 0.0f);
        vertical = vec3(0.0f, 2.0f, 0.0f);
        origin = vec3(0.0f, 0.0f, 0.0f);
    }

    ray get_ray(float u, float v) {
        return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
    }
};

#endif /* CAMERA_HH */
