#ifndef MANDELBROT_COMPLEX_H
#define MANDELBROT_COMPLEX_H

/**
 * Represents a complex number.
 */
struct Complex {
    double a; // real part
    double b; // imaginary part
};

/**
 * Complex number multiplication.
 */
struct Complex mul_complex(struct Complex x, struct Complex y) {
    struct Complex result = {x.a * y.a - x.b * y.b, x.a * y.b + x.b * y.a};
    return result;
}

/**
 * Complex number absolute value.
 */
double abs_complex(struct Complex x) {
    return sqrt(x.a * x.a + x.b * x.b);
}

/**
 * Complex number addition.
 */
struct Complex add_complex(struct Complex x, struct Complex y) {
    struct Complex result = {x.a + y.a, x.b + y.b};
    return result;
}

#endif //MANDELBROT_COMPLEX_H
