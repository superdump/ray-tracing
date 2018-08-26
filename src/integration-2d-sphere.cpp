#include <cmath>
#include <iostream>

#include "vec3.h"

vec3 random_on_unit_sphere() {
    vec3 p;
    do {
        p = 2.0f * vec3(drand48(), drand48(), drand48()) - ones;
    } while (p.squared_length() >= 1.0f);
    return unit_vector(p);
}

inline double pdf(const vec3& p) {
    return 1.0 / (4.0 * M_PI);
}

int main() {
    double sum = 0.0;
    int iterations = 0;
    int n = 1;
    const double target = 4.0 * M_PI / 3.0;
    const double epsilon = 0.0001;
    std::cout << "Target: " << target << ", epsilon: " << epsilon << "\n";
    do {
        double r = drand48();

        if (iterations == 0) {
            vec3 d = random_on_unit_sphere();
            double cosine_squared = d.z() * d.z();
            sum += cosine_squared / pdf(d);
            if (abs(target - (sum / n)) <= epsilon) {
                iterations = n;
            }
        }

        ++n;
    } while (iterations == 0);
    std::cout << "Converged to " << sum / iterations << " in " << iterations << " iterations\n";
}
