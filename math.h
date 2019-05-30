//
// Created by Nol on 12/05/2019.
//

#ifndef MANDELBROT_MATH_H
#define MANDELBROT_MATH_H

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

const Mat4f MAT4F_IDENTITY = {
        {{
                 {1, 0, 0, 0},
                 {0, 1, 0, 0},
                 {0, 0, 1, 0},
                 {0, 0, 0, 1},
         }}
};

Mat4f scale(Vec3f s) {
    return (Mat4f) {{{
                             {s.x, 0, 0, 0},
                             {0, s.y, 0, 0},
                             {0, 0, s.z, 0},
                             {0, 0, 0, 1}
                     }}};
}

Mat4f translate(Vec3f t) {
    return (Mat4f) {{{
                             {1, 0, 0, 0},
                             {0, 1, 0, 0},
                             {0, 0, 1, 0},
                             {t.x, t.y, t.z, 1}
                     }}};
}

Mat4f cross(Mat4f a, Mat4f b) {
    Mat4f result = {};

    for (uint8_t column = 0; column < 4; column++) {
        for (uint8_t row = 0; row < 4; row++) {
            for (uint8_t i = 0; i < 4; i++) {
                result.val[column][row] += a.val[i][row] * b.val[column][i];
            }
        }
    }

    return result;
}

#endif //MANDELBROT_MATH_H
