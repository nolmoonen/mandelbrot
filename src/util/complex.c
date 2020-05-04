#include "complex.h"
#include "math.h"

complex_t complex_mul(complex_t x, complex_t y)
{
    complex_t result = {x.a * y.a - x.b * y.b, x.a * y.b + x.b * y.a};
    return result;
}

complex_t complex_mul_self(complex_t x)
{
    complex_t result = {x.a * x.a - x.b * x.b, 2 * x.b * x.a};
    return result;
}

double complex_abs(complex_t x)
{
    return sqrt(x.a * x.a + x.b * x.b);
}

complex_t complex_add(complex_t x, complex_t y)
{
    complex_t result = {x.a + y.a, x.b + y.b};
    return result;
}