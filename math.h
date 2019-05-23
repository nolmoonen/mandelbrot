//
// Created by Nol on 12/05/2019.
//

#ifndef MANDELBROT_MATH_H
#define MANDELBROT_MATH_H

typedef struct vec2f {
    float x;
    float y;
} vec2f;

typedef struct vec3f {
    float x;
    float y;
    float z;
} vec3f;

typedef struct vec4f {
    float x;
    float y;
    float z;
    float w;
} vec4f;

typedef struct mat4f {
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
} mat4f;

const mat4f MAT4F_IDENTITY = {
        {{
                 {1, 0, 0, 0},
                 {0, 1, 0, 0},
                 {0, 0, 1, 0},
                 {0, 0, 0, 1},
         }}
};

mat4f scale(vec3f s) {
    return (mat4f) {{{
                             {s.x, 0, 0, 0},
                             {0, s.y, 0, 0},
                             {0, 0, s.z, 0},
                             {0, 0, 0, 1}
                     }}};
}

mat4f translate(vec3f t) {
    return (mat4f) {{{
                             {1, 0, 0, 0},
                             {0, 1, 0, 0},
                             {0, 0, 1, 0},
                             {t.x, t.y, t.z, 1}
                     }}};
}

mat4f mul(mat4f A, mat4f B) {
    mat4f result = {};

    for (uint8_t column = 0; column < 4; column++) {
        for (uint8_t row = 0; row < 4; row++) {
            for (uint8_t i = 0; i < 4; i++) {
                result.val[column][row] += A.val[i][row] * B.val[column][i];
            }
        }
    }

    return result;
}

#endif //MANDELBROT_MATH_H
