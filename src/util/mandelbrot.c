#include <stdint.h>
#include <malloc.h>
#include "complex.h"
#include "mandelbrot.h"
#include "math.h"
#include "nm_math.h"

color_t color(double m, double *hues, uint32_t max_iterations)
{
    color_t hsv;
    hsv.h = 255 - (uint32_t) (255 * nm_lerp(hues[(uint32_t) floor(m)], hues[(uint32_t) ceil(m)], fmod(m, 1)));
    hsv.s = 255;
    hsv.v = m < max_iterations ? 255 : 0;
    color_t rgb = HSVtoRGB(hsv);
    return rgb;
}

double mandelbrot(complex_t c, uint32_t max_iterations)
{
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

void generate(Texture *p_texture, Fractal p_fractal, uint32_t p_max_iterations)
{
    double *all_iterations = (double *) malloc(sizeof(double) * p_texture->width * p_texture->height);
    uint32_t *histogram = calloc(p_max_iterations - 1, sizeof(uint32_t));
    uint32_t total = 0;

    /*
     * calculate iterations
     */
    double re_diff = p_fractal.re_end - p_fractal.re_start;
    double im_diff = p_fractal.im_end - p_fractal.im_start;
    for (uint32_t y = 0; y < p_texture->height; ++y) {
        for (uint32_t x = 0; x < p_texture->width; ++x) {
            // convert pixel coordinate to complex number
            complex_t c = {
                    p_fractal.re_start + (x / (double) p_texture->width) * re_diff,
                    p_fractal.im_start + (y / (double) p_texture->height) * im_diff,
            };

            // compute number of iterations
            double m = mandelbrot(c, p_max_iterations);
            all_iterations[y * p_texture->width + x] = m;

            if (m < p_max_iterations) {
                histogram[(uint32_t) floor(m)]++;
                total++;
            }
        }
    }

    double *hues = (double *) malloc(sizeof(double) * (p_max_iterations + 1));
    double h = 0;
    for (uint32_t i = 0; i < p_max_iterations; ++i) {
        h += histogram[i] / (double) total;
        hues[i] = h;
    }
    hues[p_max_iterations] = h;

    /*
     * calculate and set colors
     */
    for (uint32_t y = 0; y < p_texture->height; ++y) {
        for (uint32_t x = 0; x < p_texture->width; ++x) {
            // create color, based on the number of iterations
            color_t rgb = color(all_iterations[y * p_texture->width + x], hues, p_max_iterations);

            // fill data
            uint32_t pixel = (y * p_texture->width + x) * 4;
            p_texture->data[pixel + 0] = rgb.r;
            p_texture->data[pixel + 1] = rgb.g;
            p_texture->data[pixel + 2] = rgb.b;
            p_texture->data[pixel + 3] = 255; // a
        }
    }

    free(hues);
    free(all_iterations);
}