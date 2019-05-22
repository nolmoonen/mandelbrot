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
                 {1.0f, 0.0f, 0.0f, 0.0f},
                 {0.0f, 1.0f, 0.0f, 0.0f},
                 {0.0f, 0.0f, 1.0f, 0.0f},
                 {0.0f, 0.0f, 0.0f, 1.0f},
         }}
};

#endif //MANDELBROT_MATH_H
