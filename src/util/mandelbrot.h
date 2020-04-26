#ifndef MANDELBROT_MANDELBROT_H
#define MANDELBROT_MANDELBROT_H

#include <stdint.h>
#include "color.h"
#include "complex.h"
#include "math.h"

// defines a fractal through coordinates
typedef struct Fractal {
    double re_start;
    double re_end;
    double im_start;
    double im_end;
} Fractal;

// mandelbrot variables for defined RESOLUTION
static const Fractal FRACTAL_START = {
        -2, // RE_START
        1, // RE_END
        -1, // IM_START
        1, // IM_END
};

typedef struct Texture {
    uint8_t *data;
    uint32_t width;
    uint32_t height;
} Texture;

/** Calculates a HSV color based on iterations. */
color_t color(double m, double *hues, uint32_t max_iterations);

/** Calculates iterations to converge, given a complex number. */
double mandelbrot(complex_t c, uint32_t max_iterations);

void generate(Texture *p_texture, Fractal p_fractal, uint32_t p_max_iterations);

#endif //MANDELBROT_MANDELBROT_H
