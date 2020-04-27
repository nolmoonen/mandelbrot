// nolmoonen v1.0.0
#ifndef NM_MATH_H
#define NM_MATH_H

#define NM_PI 3.14159265358979323846f

/** Clamp x between a and b. */
float nm_clampf(float a, float b, float x);

double nm_clamp(double a, double b, double x);

/** Perform linear interpolation between a and b. */
float nm_lerpf(float a, float b, float t);

double nm_lerp(double a, double b, double t);

#endif //NM_MATH_H
