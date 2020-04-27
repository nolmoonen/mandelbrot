#include "complex.h"
#include "math.h"

complex_t mul_complex(complex_t x, complex_t y)
{
    complex_t result = {x.a * y.a - x.b * y.b, x.a * y.b + x.b * y.a};
    return result;
}

double abs_complex(complex_t x)
{
    return sqrt(x.a * x.a + x.b * x.b);
}

complex_t add_complex(complex_t x, complex_t y)
{
    complex_t result = {x.a + y.a, x.b + y.b};
    return result;
}