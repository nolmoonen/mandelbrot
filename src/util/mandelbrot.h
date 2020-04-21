#ifndef MANDELBROT_MANDELBROT_H
#define MANDELBROT_MANDELBROT_H

#include <stdint.h>
#include "color.h"
#include "complex.h"
#include "math.h"

/**
 * Calculates a HSV color based on iterations.
 */
color_t color(double m, double *hues, uint32_t max_iterations);

/**
 * Calculates iterations to converge, given a complex number.
 */
double mandelbrot(complex_t c, uint32_t max_iterations);

#endif //MANDELBROT_MANDELBROT_H
