#ifndef MANDELBROT_ORTO_MODEL_H
#define MANDELBROT_ORTO_MODEL_H

#include <glad/glad.h>

typedef struct {
    GLuint vao;
    GLuint vbo_pos;
    GLuint vbo_tex;
} quad;

int create_quad(quad *p_quad);

int delete_quad(quad *p_quad);

#endif //MANDELBROT_ORTO_MODEL_H