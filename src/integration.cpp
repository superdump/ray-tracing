#include <cmath>
#include <iostream>

inline double pdf1(double x) {
    return 0.5 * x;
}

inline double pdf2(double x) {
    return 0.5;
}

inline double pdf3(double x) {
    return 3.0 * x * x / 8.0;
}

int main() {
    double sums[3] = {0.0, 0.0, 0.0};
    int iterations[3] = {0, 0, 0};
    int n = 1;
    const double target = 2.66666666666666666666666666666666666667;
    const double epsilon = 0.0001;
    std::cout << "Target: " << target << ", epsilon: " << epsilon << "\n";
    do {
        double r = drand48();

        if (iterations[0] == 0) {
            double x1 = sqrt(4.0 * r);
            sums[0] += x1 * x1 / pdf1(x1);
            if (abs(target - (sums[0] / n)) <= epsilon) {
                iterations[0] = n;
            }
        }

        if (iterations[1] == 0) {
            double x2 = 2.0 * r;
            sums[1] += x2 * x2 / pdf2(x2);
            if (abs(target - (sums[1] / n)) <= epsilon) {
                iterations[1] = n;
            }
        }

        if (iterations[2] == 0) {
            double x3 = pow(8.0 * r, 1.0 / 3.0);
            sums[2] += x3 * x3 / pdf3(x3);
            if (abs(target - (sums[2] / n)) <= epsilon) {
                iterations[2] = n;
            }
        }

        ++n;
    } while (iterations[0] == 0 || iterations[1] == 0 || iterations[2] == 0);
    std::cout << "pdf = 0.5x converged to " << sums[0] / iterations[0] << " in " << iterations[0] << " iterations\n";
    std::cout << "pdf = 0.5 converged to " << sums[1] / iterations[1] << " in " << iterations[1] << " iterations\n";
    std::cout << "pdf = (3/8)x^2 converged to " << sums[2] / iterations[2] << " in " << iterations[2] << " iterations\n";
}
