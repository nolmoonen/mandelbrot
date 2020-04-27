#ifndef MANDELBROT_COMPLEX_H
#define MANDELBROT_COMPLEX_H

/** Represents a complex number. */
typedef struct {
    double a; // real part
    double b; // imaginary part
} complex_t;

/** Complex number multiplication. */
complex_t mul_complex(complex_t x, complex_t y);

/** Complex number absolute value. */
double abs_complex(complex_t x);

/** Complex number addition. */
complex_t add_complex(complex_t x, complex_t y);

#endif //MANDELBROT_COMPLEX_H
