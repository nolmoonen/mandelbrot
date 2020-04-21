#include "orto_model.h"
#include <stdlib.h>

const GLfloat QUAD_POS[] = {
        -1.0f, 1.0f,
        1.0f, -1.0f,
        -1.0f, -1.0f,

        -1.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, -1.0f
};

const GLfloat QUAD_TEX[] = {
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
};

int create_quad(quad *p_quad) {
    // create VAO
    glGenVertexArrays(1, &p_quad->vao);
    glBindVertexArray(p_quad->vao);

    // create geometric vertex VBO
    glGenBuffers(1, &p_quad->vbo_pos);
    glBindBuffer(GL_ARRAY_BUFFER, p_quad->vbo_pos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_POS), QUAD_POS, GL_STATIC_DRAW);
    // index = 0, size = 2
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0);

    // create texture vertex VBO
    glGenBuffers(1, &p_quad->vbo_tex);
    glBindBuffer(GL_ARRAY_BUFFER, p_quad->vbo_tex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_TEX), QUAD_TEX, GL_STATIC_DRAW);
    // index = 1, size = 2
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0);

    glBindVertexArray(0);

    return EXIT_SUCCESS;
}

int delete_quad(quad *p_quad) {
    glDeleteBuffers(1, &p_quad->vbo_tex);
    glDeleteBuffers(1, &p_quad->vbo_pos);
    glDeleteVertexArrays(1, &p_quad->vao);

    return 0;
}
