// nolmoonen v1.0.0
#include <math.h>

#include "nm_math.h"

float nm_clampf(float a, float b, float x) {
    return fmaxf(a, fminf(b, x));
}

double nm_clamp(double a, double b, double x) {
    return fmax(a, fmin(b, x));
}

float nm_lerpf(float a, float b, float t) {
    return a * (1 - t) + b * t;
}

double nm_lerp(double a, double b, double t) {
    return a * (1 - t) + b * t;
}
