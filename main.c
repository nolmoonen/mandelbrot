#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include "bmp.h"
#include "complex.h"
#include "color.h"

// limit to the number of iterations in the mandelbrot function
const uint32_t MAX_ITER = 400;

// mandelbrot variables
const int32_t RE_START = -2;
const int32_t RE_END = 1;
const int32_t IM_START = -1;
const int32_t IM_END = 1;

// output file dimensions
const uint32_t WIDTH = 1920 * 2;
const uint32_t HEIGHT = 1080 * 2;

/**
 * Calculates a HSV color based on iterations.
 */
struct RGB color(uint32_t n, uint32_t N) {
    struct HSV hsv = {0};
    hsv.h = 255 * n / N;
    hsv.s = 255;
    hsv.v = n < N ? 255 : 0;
    struct RGB rgb = HSVtoRGB(hsv);
    return rgb;
}

/**
 * Calculates a RGB color based on iterations and a specified base color.
 */
struct RGB basecolor(uint32_t n, uint32_t N) {
    const struct RGB BASE = {0, 0, 255};
    const uint32_t THRESHOLD = 2;

    if (n > N / THRESHOLD) {
        // middle, always black
        struct RGB rgb = {0, 0, 0};
        return rgb;
    } else {
        // (2 * n < N)
        // edges, change from black to blue
        struct RGB rgb = {(n * BASE.r / N) * THRESHOLD, (n * BASE.g / N) * THRESHOLD, (n * BASE.b / N) * THRESHOLD};
        return rgb;
    }
}

/**
 * Calculates iterations to converge, given a complex number.
 */
uint32_t mandelbrot(struct Complex c) {
    struct Complex z = {0, 0};
    uint32_t n = 0;

    while (abs_complex(z) <= 2 && n < MAX_ITER) {
        z = add_complex(mul_complex(z, z), c);
        ++n;
    }

    return n;
}

int main() {
    // allocate output data
    struct IntColor *data = (struct IntColor *) malloc(sizeof(struct IntColor) * WIDTH * HEIGHT);

    for (uint32_t x = 0; x < WIDTH; ++x) {
        for (uint32_t y = 0; y < HEIGHT; ++y) {
            // convert pixel coordinate to complex number
            struct Complex c = {
                    RE_START + (x / (double) WIDTH) * (RE_END - RE_START),
                    IM_START + (y / (double) HEIGHT) * (IM_END - IM_START)
            };

            // compute number of iterations
            uint32_t m = mandelbrot(c);

            // create color, based on the number of iterations
            struct RGB rgb = basecolor(m, MAX_ITER);

            // fill data
            struct IntColor *intColor = data + y * WIDTH + x;
            intColor->a = 255;
            intColor->r = rgb.r;
            intColor->g = rgb.g;
            intColor->b = rgb.b;
        }
    }

    // create the output file
    struct Bitmap bitmap = createBitmap(data, WIDTH, HEIGHT);
    saveOutBitmap(bitmap, "test.bmp");
}