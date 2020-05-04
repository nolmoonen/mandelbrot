#ifndef MANDELBROT_COMPLEX_H
#define MANDELBROT_COMPLEX_H

/** Represents a complex number. */
typedef struct {
    double a; // real part
    double b; // imaginary part
} complex_t;

/** Complex number multiplication. */
complex_t complex_mul(complex_t x, complex_t y);

/** Complex number multiplication with itself. */
complex_t complex_mul_self(complex_t x);

/** Complex number absolute value. */
double complex_abs(complex_t x);

/** Complex number addition. */
complex_t complex_add(complex_t x, complex_t y);

#endif //MANDELBROT_COMPLEX_H
