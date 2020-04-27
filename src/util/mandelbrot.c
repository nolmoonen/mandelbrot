#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include "complex.h"
#include "mandelbrot.h"
#include "math.h"
#include "nm_math.h"

color_t color(float m, const float *const hues, uint32_t max_iterations)
{
    float hue_lo = hues[(uint32_t) floorf(m)];
    float hue_hi = hues[(uint32_t) ceilf(m)];

    color_t hsv;
    hsv.h = 255 - (uint32_t) (255 * nm_lerpf(hue_lo, hue_hi, fmodf(m, 1)));
    hsv.s = 255;
    hsv.v = m < max_iterations ? 255 : 0;
    color_t rgb = HSVtoRGB(hsv);

    return rgb;
}

float mandelbrot(complex_t c, uint32_t max_iterations)
{
    complex_t z = {0, 0};
    uint32_t n = 0;

    while (abs_complex(z) <= 2 && n < max_iterations) {
        z = add_complex(mul_complex(z, z), c);
        n++;
    }

    if (n == max_iterations) {
        return max_iterations;
    }

    // fractional iteration count (http://linas.org/art-gallery/escape/escape.html)
    return nm_clampf(0, max_iterations, (float) n + 1.f - logf(log2f((float) abs_complex(z))));
}

void generate(volatile Texture *p_texture, Fractal p_fractal, uint32_t p_max_iterations)
{
    float *all_iterations = malloc(sizeof(float) * p_texture->width * p_texture->height);
    // histogram counting the frequencies of all values expect max
    uint32_t *histogram = calloc(p_max_iterations, sizeof(uint32_t));
    // number of pixels that have less iterations than max
    uint32_t total = 0;

    /** calculate iterations */
    double re_step = (p_fractal.re_end - p_fractal.re_start) / p_texture->width;
    double im_step = (p_fractal.im_end - p_fractal.im_start) / p_texture->height;

    double im = p_fractal.im_start; // imaginary part of pixel (x, y)
    for (uint32_t y = 0; y < p_texture->height; y++) {
        double re = p_fractal.re_start; // real part of pixel (x, y)
        for (uint32_t x = 0; x < p_texture->width; x++) {
            // convert pixel coordinate to complex number
            complex_t c = {re, im};

            // compute number of iterations
            float m = mandelbrot(c, p_max_iterations);
            all_iterations[y * p_texture->width + x] = m;

            if (m < p_max_iterations) {
                histogram[(uint32_t) floorf(m)]++;
                total++;
            }

            re += re_step;
        }
        im += im_step;
    }

    /** construct a hue map for all possible values */
    float *hues = malloc(sizeof(float) * (p_max_iterations + 1));
    float h = 0;
    for (uint32_t i = 0; i < p_max_iterations; i++) {
        h += histogram[i] / (float) total;
        hues[i] = h;
    }
    hues[p_max_iterations] = h;
    free(histogram);

    /** lookup color and create pixel data */
    uint32_t pixel_start = 0;
    for (uint32_t y = 0; y < p_texture->height; y++) {
        for (uint32_t x = 0; x < p_texture->width; x++) {
            // create color, based on the number of iterations
            color_t rgb = color(all_iterations[y * p_texture->width + x], hues, p_max_iterations);
            memcpy(&p_texture->data[pixel_start], &rgb, sizeof(rgb));
            pixel_start += sizeof(color_t);
        }
    }

    free(hues);
    free(all_iterations);
}