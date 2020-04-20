#ifndef MANDELBROT_NMMATH_H
#define MANDELBROT_NMMATH_H

typedef struct Vec2f {
    float x;
    float y;
} Vec2f;

typedef struct Vec3f {
    float x;
    float y;
    float z;
} Vec3f;

typedef struct Vec4f {
    float x;
    float y;
    float z;
    float w;
} Vec4f;

typedef struct Mat4f {
    union {
        float val[4][4];

        struct {
            float m00;
            float m01;
            float m02;
            float m03;

            float m10;
            float m11;
            float m12;
            float m13;

            float m20;
            float m21;
            float m22;
            float m23;

            float m30;
            float m31;
            float m32;
            float m33;
        };
    };
} Mat4f;

extern const Mat4f MAT4F_IDENTITY;

/** Returns a scaling matrix based on scaling vector s. */
Mat4f scale(Vec3f s);

/** Returns a translation matrix based on translation vector t. */
Mat4f translate(Vec3f t);

/** Returns the cross product of matrices a and b. */
Mat4f cross(Mat4f a, Mat4f b);

/** Perform linear interpolation between a and b. */
double lerp(double a, double b, double t);

#endif //MANDELBROT_NMMATH_H
