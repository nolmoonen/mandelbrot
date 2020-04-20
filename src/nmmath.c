#include <stdint.h>
#include "nmmath.h"

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

    for (uint32_t column = 0; column < 4; column++) {
        for (uint32_t row = 0; row < 4; row++) {
            for (uint32_t i = 0; i < 4; i++) {
                result.val[column][row] += a.val[i][row] * b.val[column][i];
            }
        }
    }

    return result;
}

double lerp(double a, double b, double t) {
    return a * (1 - t) + b * t;
}