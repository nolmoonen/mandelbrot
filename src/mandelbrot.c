#include <stdint.h>
#include "complex.h"
#include "mandelbrot.h"
#include "nmmath.h"
#include "math.h"

color_t color(double m, double *hues, uint32_t max_iterations) {
    color_t hsv;
    hsv.h = 255 - (uint32_t) (255 * lerp(hues[(uint32_t) floor(m)], hues[(uint32_t) ceil(m)], fmod(m, 1)));
    hsv.s = 255;
    hsv.v = m < max_iterations ? 255 : 0;
    color_t rgb = HSVtoRGB(hsv);
    return rgb;
}

double mandelbrot(complex_t c, uint32_t max_iterations) {
    complex_t z = {0, 0};
    uint32_t n = 0;

    while (abs_complex(z) <= 2 && n < max_iterations) {
        z = add_complex(mul_complex(z, z), c);
        ++n;
    }

    if (n == max_iterations) {
        return max_iterations;
    }

    return n + 1 - log(log2(abs_complex(z)));
}